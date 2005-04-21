/* The system call implemented in this file:
 *   m_type:	SYS_SIGRETURN
 *
 * The parameters for this system call are:
 *    m2_i1:	SIG_PROC		(process number)
 *    m2_i3:	SIG_FLAGS		(sig return flags) (unused)
 *    m2_p1:	SIG_CTXT_PTR	(pointer to sigcontext structure)
 */

#include "../kernel.h"
#include "../system.h"
#include <signal.h>
#include <sys/sigcontext.h>
INIT_ASSERT

/*===========================================================================*
 *			      do_sigreturn				     *
 *===========================================================================*/
PUBLIC int do_sigreturn(m_ptr)
register message *m_ptr;
{
/* POSIX style signals require sys_sigreturn to put things in order before the
 * signalled process can resume execution
 */

  struct sigcontext sc;
  register struct proc *rp;
  phys_bytes src_phys;

  rp = proc_addr(m_ptr->SIG_PROC);
  if (! isuserp(rp)) 
  	printf("message source: %d; rp: %d\n", m_ptr->m_source, rp->p_nr);
  assert(isuserp(rp));

  /* Copy in the sigcontext structure. */
  src_phys = umap_local(rp, D, (vir_bytes) m_ptr->SIG_CTXT_PTR,
		  (vir_bytes) sizeof(struct sigcontext));
  if (src_phys == 0) return(EFAULT);
  phys_copy(src_phys, vir2phys(&sc), (phys_bytes) sizeof(struct sigcontext));

  /* Make sure that this is not just a jmp_buf. */
  if ((sc.sc_flags & SC_SIGCONTEXT) == 0) return(EINVAL);

  /* Fix up only certain key registers if the compiler doesn't use
   * register variables within functions containing setjmp.
   */
  if (sc.sc_flags & SC_NOREGLOCALS) {
	rp->p_reg.retreg = sc.sc_retreg;
	rp->p_reg.fp = sc.sc_fp;
	rp->p_reg.pc = sc.sc_pc;
	rp->p_reg.sp = sc.sc_sp;
	return (OK);
  }
  sc.sc_psw  = rp->p_reg.psw;

#if (CHIP == INTEL)
  /* Don't panic kernel if user gave bad selectors. */
  sc.sc_cs = rp->p_reg.cs;
  sc.sc_ds = rp->p_reg.ds;
  sc.sc_es = rp->p_reg.es;
#if _WORD_SIZE == 4
  sc.sc_fs = rp->p_reg.fs;
  sc.sc_gs = rp->p_reg.gs;
#endif
#endif

  /* Restore the registers. */
  memcpy(&rp->p_reg, (char *)&sc.sc_regs, sizeof(struct sigregs));

  return(OK);
}

