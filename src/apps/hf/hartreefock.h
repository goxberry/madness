//#ifndef HARTREEFOCK_H_
//#define HARTREEFOCK_H_

#include <mra/mra.h>
#include <world/world.h>
#include <vector>

#include "eigsolver.h"

namespace madness
{
    
  //***************************************************************************
  // TYPEDEFS
  typedef SharedPtr< FunctionFunctorInterface<double,3> > functorT;
  typedef Function<double,3> funcT;
  typedef Vector<double,3> coordT;
  //***************************************************************************

  //***************************************************************************
  class HartreeFockNuclearPotentialOp : public EigSolverOp
  {
  public:
    //*************************************************************************
    // Constructor
    HartreeFockNuclearPotentialOp(World& world, funcT V, double coeff, double thresh);
    //*************************************************************************

    //*************************************************************************
    // Is there an orbitally-dependent term?
    virtual bool is_od() {return false;}
    //*************************************************************************

    //*************************************************************************
    // Is there a density-dependent term?
    virtual bool is_rd() {return true;}
    //*************************************************************************

    //*************************************************************************
    virtual funcT op_o(const std::vector<funcT>& phis, const funcT& psi) {}
    //*************************************************************************

    //*************************************************************************
    virtual funcT op_r(const funcT& rho, const funcT& psi);
    //*************************************************************************

  private:
    //*************************************************************************
    funcT _V;
    //*************************************************************************

    //*************************************************************************
    functorT ones;
    functorT zeros;
    //*************************************************************************
  };
  //***************************************************************************

  //***************************************************************************
  class HartreeFockCoulombOp : public EigSolverOp
  {
  public:
    //*************************************************************************
    // Constructor
    HartreeFockCoulombOp(World& world, double coeff, double thresh);
    //*************************************************************************

    //*************************************************************************
    // Is there an orbitally-dependent term?
    virtual bool is_od() {return false;}
    //*************************************************************************

    //*************************************************************************
    // Is there a density-dependent term?
    virtual bool is_rd() {return true;}
    //*************************************************************************

    //*************************************************************************
    virtual funcT op_o(const std::vector<funcT>& phis, const funcT& psi) {}
    //*************************************************************************

    //*************************************************************************
    virtual funcT op_r(const funcT& rho, const funcT& psi);
    //*************************************************************************

  private:
    //*************************************************************************
    functorT ones;
    functorT zeros;
    //*************************************************************************
  };
  //***************************************************************************

  //***************************************************************************
  class HartreeFockExchangeOp : public EigSolverOp
  {
  public:
    //*************************************************************************
    // Constructor
    HartreeFockExchangeOp(World& world, double coeff, double thresh);
    //*************************************************************************

    //*************************************************************************
    // Is there an orbitally-dependent term?
    virtual bool is_od() {return true;}
    //*************************************************************************

    //*************************************************************************
    // Is there a density-dependent term?
    virtual bool is_rd() {return false;}
    //*************************************************************************

    //*************************************************************************
    virtual funcT op_o(const std::vector<funcT>& phis, const funcT& psi);
    //*************************************************************************

    //*************************************************************************
    virtual funcT op_r(const funcT& rho, const funcT& psi) {}
    //*************************************************************************

  private:
    //*************************************************************************
    functorT ones;
    functorT zeros;
    //*************************************************************************
  };
  //***************************************************************************

  //***************************************************************************
  class HartreeFock : public IEigSolverObserver
  {
  public:
  
    //*************************************************************************
    // Constructor
    HartreeFock(World& world, funcT V, std::vector<funcT> phis, 
      std::vector<double> eigs, bool bCoulomb, bool bExchange, double thresh);
    //*************************************************************************
  
    //*************************************************************************
    // Constructor for ground state only
    HartreeFock(World& world, funcT V, funcT phi, double eig, 
      bool bCoulomb, bool bExchange, double thresh);
    //*************************************************************************
  
    //*************************************************************************
    // Destructor
  	virtual ~HartreeFock();
    //*************************************************************************
  
    //*************************************************************************
  	void hartree_fock(int maxits);
    //*************************************************************************
  
    //*************************************************************************
    funcT calculate_coulomb(funcT psi);
    //*************************************************************************

    //*************************************************************************
    funcT calculate_exchange(funcT psi);
    //*************************************************************************

    //*************************************************************************
    double calculate_ke_sp(funcT psi);
    //*************************************************************************

    //*************************************************************************
    double calculate_pe_sp(funcT psi);
    //*************************************************************************

    //*************************************************************************
    double calculate_coulomb_energy(const std::vector<funcT>& phis,
        const funcT& psi);
    //*************************************************************************

    //*************************************************************************
    double calculate_exchange_energy(const std::vector<funcT>& phis,
        const funcT& psi);
    //*************************************************************************

  	//*************************************************************************
    virtual void iterateOutput(const std::vector<funcT>& phis,
        const std::vector<double>& eigs, const int& iter);
    //*************************************************************************

    //*************************************************************************
    bool include_coulomb()
    {
      return _bCoulomb;
    }
    //*************************************************************************

    //*************************************************************************
    bool include_exchange()
    {
      return _bExchange;
    }
    //*************************************************************************

    //*************************************************************************
    double get_eig(int indx)
    {
      return _solver->get_eig(indx);
    }
    //*************************************************************************

    //*************************************************************************
    funcT get_phi(int indx)
    {
      return _solver->get_phi(indx);
    }
    //*************************************************************************
    
    //*************************************************************************
    double calculate_tot_ke_sp(const std::vector<funcT>& phis);
    //*************************************************************************

    //*************************************************************************
    double calculate_tot_pe_sp(const std::vector<funcT>& phis);
    //*************************************************************************

    //*************************************************************************
    double calculate_tot_coulomb_energy(const std::vector<funcT>& phis);
    //*************************************************************************

    //*************************************************************************
    double calculate_tot_exchange_energy(const std::vector<funcT>& phis);
    //*************************************************************************

private:

    //*************************************************************************
    // Eigenvalue solver
    EigSolver* _solver;
    //*************************************************************************
    
    //*************************************************************************
    // Flags for whether to include the coulomb and exchange
    bool _bCoulomb;
    bool _bExchange;
    //*************************************************************************

    //*************************************************************************
    World& _world;
    //*************************************************************************

    //*************************************************************************
    funcT _V;
    //*************************************************************************

    //*************************************************************************
    double _thresh;
    //*************************************************************************
    
    //*************************************************************************
    functorT ones;
    functorT zeros;
    //*************************************************************************

    //*************************************************************************
    World& world() {return _world;}
    //*************************************************************************

    //*************************************************************************
    double thresh() {return _thresh;}
    //*************************************************************************

  };
  //***************************************************************************

}

//#endif /*HARTREEFOCK_H_*/
