#define WORLD_INSTANTIATE_STATIC_TEMPLATES  

#include "eigsolver.h"
#include "util.h"

namespace madness
{

  //***************************************************************************
  EigSolver::EigSolver(World& world, std::vector<funcT> phis, 
      std::vector<double> eigs, std::vector<EigSolverOp*> ops, double thresh)
  : _phis(phis), _eigs(eigs), _ops(ops), _world(world), _thresh(thresh)
  {
    ones = functorT(new OnesFunctor());
    zeros = functorT(new ZerosFunctor());
  }
  //***************************************************************************
  
  //***************************************************************************
  EigSolver::EigSolver(World& world, funcT phi, double eig, 
      std::vector<EigSolverOp*> ops, double thresh) : _ops(ops), 
      _world(world), _thresh(thresh)
  {
    ones = functorT(new OnesFunctor());
    zeros = functorT(new ZerosFunctor());
    _phis.push_back(phi);
    _eigs.push_back(eig);
  }
  //***************************************************************************
  
  //***************************************************************************
      EigSolver::~EigSolver()
  {
    // Eigsolver is responsible for deleting the ops
    for (std::vector<EigSolverOp*>::iterator it = _ops.begin(); it != _ops.end(); 
      it++) delete (*it);
    _ops.clear();
    // Clear eigenvectors
    _phis.clear();
    // Clear eigenvalues
    _eigs.clear();
    // Clear observers
    _obs.clear();
  }
  //***************************************************************************
  
  //***************************************************************************
  void EigSolver::solve(int maxits)
  {
    for (int it = 0; it < maxits; it++)
    {
      for (unsigned int pi = 0; pi < _phis.size(); pi++)
      {
        // Get psi from collection
        funcT psi = _phis[pi];
        funcT pfunc = FunctionFactory<double,3>(_world).functor(zeros);
        // Loop through all ops
        for (unsigned int oi = 0; oi < _ops.size(); oi++)
        {
          EigSolverOp* op = _ops[oi];
          pfunc += op->coeff() * op->op(_phis, psi);
        }
        pfunc.scale(-2.0).truncate(_thresh);
        SeparatedConvolution<double,3> op = 
          BSHOperator<double,3>(_world, sqrt(-2.0*_eigs[pi]), 
              FunctionDefaults<3>::k, 1e-3, _thresh);      
        // Apply the Green's function operator (stubbed)
        funcT tmp = apply(op, pfunc);
        // (Not sure whether we have to do this mask thing or not!)
        for (unsigned int pj = 0; pj < pi; ++pj)
        {
          // Project out the lower states
          // Make sure that pi != pj
          if (pi != pj)
          {
            // Get other wavefunction
            funcT psij = _phis[pj];
            double overlap = inner(tmp, psij);
            tmp -= overlap*psij;
          }
        }
        // Update e
        funcT r = tmp - psi;
        double norm = tmp.norm2();
        double eps_old = _eigs[pi];
        double ecorrection = -0.5*inner(pfunc, r) / (norm*norm);
        double eps_new = eps_old + ecorrection;
        // Sometimes eps_new can go posivite, THIS WILL CAUSE THE ALGORITHM TO CRASH. So,
        // I bounce the new eigenvalue back into the negative side of the real axis. I 
        // keep doing this until it's good or I've already done it 10 times.
        int counter = 0;
        while (eps_new >= 0.0 && counter < 10)
        {
          // Split the difference between the new and old estimates of the 
          // pi-th eigenvalue.
          eps_new = eps_old + 0.5*(eps_new - eps_old);
          counter++;
        }
        // Still no go, forget about it. (1$ to Donnie Brasco)
        if (eps_new >= 0.0)
          {
            printf("FAILURE OF WST: exiting!!\n\n");
            _exit(0);
          }
        // Update the eigenvalue estimates and wavefunctions.
        tmp.truncate(_thresh);
        _eigs[pi] = eps_new;
        _phis[pi] = tmp.scale(1.0/tmp.norm2());
      }
      // Output to observables
      for (std::vector<IEigSolverObserver*>::iterator itr = _obs.begin(); itr
        != _obs.end(); ++itr)
      {
        (*itr)->iterateOutput(_phis, _eigs, it);
      }
    }
  }
  //***************************************************************************
}
