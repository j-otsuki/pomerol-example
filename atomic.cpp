//
// This file is a part of pomerol - a scientific ED code for obtaining
// properties of a Hubbard model on a finite-size lattice
//
// Copyright (C) 2010-2013 Andrey Antipov <antipov@ct-qmc.org>
// Copyright (C) 2010-2013 Igor Krivenko <igor@shg.ru>
//
// pomerol is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// pomerol is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with pomerol.  If not, see <http://www.gnu.org/licenses/>.

/** \file examples/example01.cpp
** \brief An example of how to use the pomerol library.
**
** \author Andrey Antipov (Andrey.E.Antipov@gmail.com)
*/

/* In this file we provide a tutorial example of how to actually use the
 * pomerol library.
 */

// Include the pomerol library
#include <pomerol.h>
#include "pomerol/LatticePresetsExtra.h"
#include "pomerol/OperatorPresetsExtra.h"
#include <fstream>
#include <stdio.h>
#include <time.h>

// Use the namespace of Pomerol. Otherwise Pomerol::{name}
// calling of the objects would be required
using namespace Pomerol;

boost::mpi::communicator world;
// Small routine to make fancy screen output for text.
void print_section (const std::string& str);

template<class T>
std::string toString(T in)
{
    std::ostringstream stream;
    stream << in;
    return stream.str();
}

typedef std::pair<double, QuantumNumbers> EigenSystem;

void fprint_eigen(std::vector<EigenSystem> &eigen)
{
    // std::ofstream f("test.dat");
    // for(int i=0; i<eigen.size(); i++){
    //     f << std::scientific << eigen[i].first << "  " << eigen[i].second << std::endl;
    // }
    // f.close();

    FILE *fp=fopen("eigenvalues.dat", "w");
    for(int i=0; i<eigen.size(); i++){
        fprintf(fp, "%.6e %s\n", eigen[i].first, toString(eigen[i].second).c_str());
    }
    fclose(fp);
}

void fprint_Gf1(GreensFunction &gf, double beta)
{
    FILE *fp=fopen("Gf.dat", "w");
    for(int i=0; i<100; i++){
        double w = (double)(2*i+1) * M_PI / beta;
        ComplexType z = gf(i);
        fprintf(fp, "%.6e %.6e %.6e\n", w, z.real(), z.imag());
    }
    fclose(fp);
}

void fprint_Gf2(TwoParticleGF &chi)
{
    FILE *fp=fopen("chi.dat", "w");
    int nm = 10;
    for(int n3 = -nm; n3<nm; ++n3){
        for(int n1 = -nm; n1<nm; ++n1){
            for(int n2 = -nm; n2<nm; ++n2){
                ComplexType z = chi(n1,n2,n3);
                fprintf(fp, "wb=%d %d %d |", n3, n1, n2);
                fprintf(fp, " %.6e %.6e\n", z.real(), z.imag());
            }
            fprintf(fp, "\n");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void print_time(clock_t start, const char *str)
{
    if (!world.rank()) {
        std::cout << "#Time: "
                  << (double)(clock()-start)/CLOCKS_PER_SEC << " sec"
                  << " (" << str << ")"<< std::endl << std::flush;
    }
}

/* Generic tips:
 * The calculation is done by computing a set of objects in the following order:
 * Lattice -> IndexClassification -> IndexHamiltonian -> Symmetrizer ->
 * -> StatesClassification -> Hamiltonian -> FieldOperator
 ;
 * (for thermal objects, such as GFs in Matsubara domain)
 * -> DensityMatrix -> Greens Function
 *                  -> TwoParticle GF   -> Vertex4
 * The detailed explanation of each class is given below.
 */

int main(int argc, char* argv[])
{
    clock_t time_start = clock(), time_temp;
    boost::mpi::environment MpiEnv(argc, argv);
    world = boost::mpi::communicator();

    /* As pomerol is an ED code, it requires a finite-size lattice to be
     * provided. Here is an example of a lattice of 2 sites. */
    // First, we construct an empty lattice
    Lattice L;
    // Add a site with a name "A", that has (2l+1) orbitals and 2 spins.
    int l_orb=3;
    L.addSite(new Lattice::Site("A", 2*l_orb+1, 2));
    // Add one more site with a name "B". It also has 1 orbitals and 2 spins.

    // Let us now connect two sites with a hopping term, with matrix element -1.
    // RealType t=1.0;
    // LatticePresets::addHopping(&L, "A","B", -t);

    /* Now add interaction. In order to provide some custom interaction, one can
     * give any custom term of 4,6 operators. This is done via
     * Lattice.addTerm(Lattice::Term) method.
     * We will use Hubbard-type n_{up}n_{down} interaction. For this and some
     * other typical interactions, such as SzSz or SS couplings a shortcut is
     * provided in the LatticePresets class.
     */
    RealType U_int = 6.0;
    RealType J_hund = 0.7;
    RealType mu = 3.0;
    RealType lambda_ls = 0.0;
    std::vector<RealType> Fk(l_orb+1, 0.);
    // Fk.push_back(U);  // F0
    // Fk.push_back(0.6);  // F2
    // Fk.push_back(0.1);  // F4
    // Fk.push_back(0.01);  // F6
    if (l_orb==1){
        Fk[0] = U_int;
        Fk[1] = 5.0 * J_hund;
    }
    else if (l_orb == 2){
        Fk[0] = U_int;
        Fk[1] = J_hund * 14.0 / (1.0 + 0.63);
        Fk[2] = 0.630 * Fk[1];
    }
    else if (l_orb == 3){
        Fk[0] = U_int;
        Fk[1] = 6435.0 * J_hund / (286.0 + 195.0 * 451.0 / 675.0 + 250.0 * 1001.0 / 2025.0);
        Fk[2] = 451.0 * Fk[1] / 675.0;
        Fk[3] = 1001.0 * Fk[1] / 2025.0;
    }
    // LatticePresets::addCoulombS adds Un_{up}n_{down} for 1 orbital and 2 spins.
    // LatticePresets::addCoulombS(&L, "A", U_int, -mu);
    // Fully rotationally invariant Coulomb interactions
    LatticePresetsExtra::addCoulombF(&L, "A", Fk, l_orb, -mu);
    LatticePresetsExtra::addSpinOrbit(&L, "A", l_orb, lambda_ls);

    // Let us now print which sites and terms are defined.
    if (!world.rank()) {
        INFO("Sites"); // equivalent to std::cout << "Sites" << std::endl;
        L.printSites();
        INFO("Terms");
        L.printTerms(2);
        INFO("Terms with 4 operators");
        L.printTerms(4);

        print_section("Indices");
    };
    /* In order to go further, we need to introduce the index space. An index
     * is a number that uniquely identifies a combination of (site,orbital,spin).
     * The object that takes care of handling indices is called
     * IndexClassification.
     */

    // Construct IndexClassification
    IndexClassification IndexInfo(L.getSiteMap());

    /* Important remark 1!
     * Most of the objects that are defined within Pomerol library have the
     * following semantics. They can be constructed, prepared and computed.
     * This means
     *   - constructed: No operations are done except from initializing links to
     *                  other objects that current class depends on.
     *   - prepared: Typically, this is when all memory allocation takes place.
     *   - computed: The actual computation. This is the most costly operation.
     *               When no heavy computation is done, it can be done during
     *               preparation stage.
     */

    // IndexClassification does not require much computing time, so everything
    // is calculated during prepare() method.
    IndexInfo.prepare();

    // Print which indices we have
    if (!world.rank())
        IndexInfo.printIndices();
    // Save the total number of indices.
    ParticleIndex IndexSize = IndexInfo.getIndexSize();

    print_section("Matrix element storage");
    /* We now need to construct the actual Hamiltonian. First, we need to create
     * it in the index space, e.g. write down a formula with all terms. It is
     * useful for a subsequent symmetry analysis. The corresponding class is called
     * an IndexHamiltonian. It is inherited from Operator class, which is designed
     * to represent a generic second-quantized fermionic operator and fully model
     * the algebra of fermionic operators. This means one can multiply, add,
     * subtract it, and also calculate the commutator of two operators, etc.
     */

    // First construct the IndexHamiltonian object.
    IndexHamiltonian Storage(&L,IndexInfo);
    // Then prepare it. As previously no long computation required, so everything
    // is done in the prepare() method.
    Storage.prepare();
    // Print out the Hamiltonian.
    if (!world.rank()) {
        INFO("Terms");
        INFO(Storage);
    }

    // Let us make a test, that our Hamiltonian commutes with an operator, that
    // represents the total number of particles. The preset for such operator is
    // defined in OperatorPresets.h and .cpp files.
    OperatorPresets::N N(IndexSize);
    if (!world.rank()) {
        INFO("N terms");
        N.printAllTerms();
        if ((Storage.commutes(N))) INFO("H commutes with N");
    }

    print_section("Symmetry");
    /* The Hamiltonian that is set now, has a set of symmetries. One can identify
     * them by checking that the IndexHamiltonian commutes with the corresponding
     * symmetry operator. The class that does it is called Symmetrizer.
     */
    // Construct Symmetrizer
    Symmetrizer Symm(IndexInfo, Storage);
    /* At this stage Symmetrizer checks symmetries with some predefined operators,
     * such as number-of-particles and sz operators.
     */
    Symm.compute();
    /* A custom check for a symmetry can be done using
     * Symmetrizer.checkSymmetry(Operator).
     * After checking all the symmetry operations, Symmetrizer defines a set of
     * quantum numbers, which uniquely identifies the closed region in the
     * phase space.
     * Calling Symmetrizer::compute(true) will skip all symmetry operations and
     * result in a single Hamiltonian block in the end.
     */

     // Lz is conserved in the absence of LS coupling
     // Jz=Lz+Sz is conserved in the presence of LS coupling
     if( lambda_ls==0. ){ // with LS coupling
         Operator op_Lz = OperatorPresets::LzSite(IndexInfo, "A");
         INFO("Lz = " << op_Lz);
         if(Symm.checkSymmetry(op_Lz))  INFO("[H, Lz] = 0");
     }
     else{ // without LS coupling
        Operator op_Sz = OperatorPresets::SzSite(IndexInfo, "A");
        Operator op_Lz = OperatorPresets::LzSite(IndexInfo, "A");
        Operator op_Jz = op_Sz + op_Lz;
        INFO("Jz = " << op_Jz);
        if(Symm.checkSymmetry(op_Jz))  INFO("[H, Jz] = 0");
    }

    INFO("Conserved quantum numbers " << Symm.getQuantumNumbers());

    print_section("States classification");
    /* We shall proceed now with obtaining the spectrum of the Hamiltonian.
     * First, introduce a basis of Fock states and classify them into blocks
     * that correspond to a set of quantum numbers. This is done in the
     * StatesClassification class. It provides all the information about Blocks
     * and FockStates.
     */
    StatesClassification S(IndexInfo,Symm);
    S.compute();

    if (!world.rank()) {
        unsigned long n_states = S.getNumberOfStates();
        INFO("Number of States is " << n_states);
        BlockNumber n_blocks = S.NumberOfBlocks();
        INFO("Number of Blocks is " << n_blocks);
        // fileout
        std::ofstream fout("states.dat");
        fout << "Number of States is " << n_states << std::endl;
        fout << "Number of Blocks is " << n_blocks << std::endl;
        fout << "Block list:\n(# size quantum_numbers)" << std::endl;
        for(BlockNumber i=0; i<n_blocks; i++){
            // INFO(S.getQuantumNumbers(i));
            fout << i << "  " << S.getBlockSize(i) << "  " << S.getQuantumNumbers(i) << std::endl;
        }
        fout.close();
    }
    world.barrier();

    print_section("Hamiltonian");
    time_temp = clock();
    /* We now convert the IndexHamiltonian into the basis of Fock States. The
     * Hamiltonian class is the one, which does it.
     */
    Hamiltonian H(IndexInfo, Storage, S);
    // Enter all blocks of the Hamiltonian.
    H.prepare();
    // Diagonalize them.
    H.compute(world);
    // Get ground energy.
    if (!world.rank())
        INFO("The value of ground energy is " << H.getGroundEnergy());

    if (!world.rank()){
        INFO("Eigenvalues");
        // create a list of pairs of eigenvalue and quantum numers
        std::vector<EigenSystem> eigen;
        for(BlockNumber i=0; i<S.NumberOfBlocks(); i++){
            HamiltonianPart H_part = H.getPart(i);
            // INFO(H_part.getQuantumNumbers());
            // INFO(H_part.getEigenValues());
            QuantumNumbers q = H_part.getQuantumNumbers();
            for(InnerQuantumState j=0; j<H_part.getSize(); j++){
                eigen.push_back( std::make_pair( H_part.getEigenValue(j), q ) );
            }
        }
        // sort eigenvalues in ascending order
        std::sort(eigen.begin(), eigen.end());
        // print all eigenvalues and corresponding quantum numbers
        std::ofstream fout("eigenvalues.dat");
        for(int i=0; i<eigen.size(); i++)
            fout << eigen[i].first << "  " << eigen[i].second << std::endl;
        fout.close();
        // fprint_eigen(eigen);
    }
    world.barrier();
    print_time(time_temp, "eigenstates");

    /* Important remark 2!
     * All the calculations done in the pomerol code are done with respect to the
     * block structure of the Hamiltonian. All objects that operate in Fock
     * space and all thermal objects, such as Green's functions are in fact a
     * set of pieces (called "parts") that operate on a certain block or set of
     * blocks. As such all actual computations are done within this parts and
     * grand objects like Green's functions or Hamiltonian basically just loops
     * over parts and tells them to run prepare() or compute() methods.
     */

    /* At this stage the Hamiltonian, defined in Fock Space is
     * entered and diagonalized and it's spectrum and eigenfunctions can be
     * directly accessed to calculate some observables.
     *
     * We shall now proceed to the calculations of thermal quantities, e.g.
     * assume that our finite-size system was adiabatically connected to a thermal
     * reservoir, that sets certain temperature (in fact, inverse temperature
     * beta). This means, that the observables in the systems should be calculated
     * with a density-matrix exp(-\beta H), rather than by averaging with the
     * ground state. In the eigenbasis of the Hamiltonian the calculation of
     * a density matrix is straightforward - it is just \exp(-\beta (E_i - E_0)),
     * where E_i is an energy of the excited state, and E_0 is the ground energy.
     * The procedure is done as following:
     */

    print_section("Density Matrix");
    // Define inverse temperature
    RealType beta = 10.0;

    // Create the Density Matrix.
    DensityMatrix rho(S,H,beta);
    // Allocate all internal parts of density matrix.
    rho.prepare();
    // Actually compute the density matrix.
    rho.compute();

    RealType DensityMatrixCutoff = 1e-15;
    if (!world.rank()){
        RealType sum=0;  // to be 1.0
        int n_state_reduced=0;
        for (QuantumState i=0; i<S.getNumberOfStates(); ++i){
            RealType w = rho.getWeight(i);
            if ( w > DensityMatrixCutoff ){
                // INFO( w << "   " << S.getQuantumNumbers(S.getBlockNumber(i)) );
                sum += w;
                ++n_state_reduced;
            }
        }
        INFO("Number of states having non-negligible weight: " << n_state_reduced);
        INFO("Total weight of the trucated states: " << sum-1.0);
        // fileout
        std::ofstream fout("density_matrix.dat");
        fout << "Number of states having non-negligible weight: " << n_state_reduced << std::endl;
        fout << "Total weight of the trucated states: " << sum-1.0 << std::endl;
        for (QuantumState i=0; i<S.getNumberOfStates(); ++i){
            RealType w = rho.getWeight(i);
            if ( w > DensityMatrixCutoff )
                fout << w << "   " << S.getQuantumNumbers(S.getBlockNumber(i)) << std::endl;
        }
        fout.close();
    }

    rho.truncateBlocks(DensityMatrixCutoff);

    if (!world.rank()) {
        // fileout
        std::ofstream fout("states_retained.dat");
        fout << "Block retained:\n(# size quantum_numbers)" << std::endl;
        for(BlockNumber i=0; i<S.NumberOfBlocks(); i++){
            // INFO(S.getQuantumNumbers(i));
            if (rho.isRetained(i))
                fout << i << "  " << S.getBlockSize(i) << "  " << S.getQuantumNumbers(i) << std::endl;
        }
        fout.close();
    }
    world.barrier();

    print_section("Creation/Annihilation operators");
    time_temp = clock();
    /* Lehmanns representation of the Green's function required creation and
     * annihilation operators, calculated in the basis of eigenstates of the
     * Hamiltonian. Creation/AnnihilationOperator are the classes that do it.
     */

    // Let us create c^+_{"A",up} and c_{"A",up}
    ParticleIndex up_index = IndexInfo.getIndex("A",0,up);
    CreationOperator CX(IndexInfo,S,H,up_index);
    CX.prepare();
    CX.compute();
    AnnihilationOperator C(IndexInfo,S,H,up_index);
    C.prepare();
    C.compute();

    world.barrier();
    print_time(time_temp, "Creation/Annihilation op");

    print_section("Single-particle Green function");
    time_temp = clock();
// The local Greens function in the Matsubara domain G_{"A",up}(i\omega_n)
    GreensFunction GF(S,H,C,CX, rho);
    GF.prepare();
    /* Calculate the GF and cache values for 10 Matsubaras.
     * These values will be fast to retrieve. The other values are also
     * accessible, but will require a short calculation to be done.
     */
    GF.compute();

    if (!world.rank()){
        for(int n = 0; n<10; ++n)
            INFO(n << " | " << GF(n));
        fprint_Gf1(GF, beta);
    }
    world.barrier();
    print_time(time_temp, "GF");

    print_section("Two-particle Green functions");
    /* The two particle GF is constructed in analogy to the single-particle GF,
     * it requires 4 operators to be provided though.
     */
    TwoParticleGF Chi(S,H,C,C,CX,CX,rho);
    /* Some knobs to make calc faster - the larger the values of tolerances, the faster is calc, but rounding errors may show.
       Typically these errors are less then 10^{-3} of the value.
       Here are some knobs that give very high-precision. If you want to make things faster, and when many values on
       different frequencies required - change ReduceResonanceTolerance to something like 10^-4. */
    /** A difference in energies with magnitude less than this value is treated as zero. */
    Chi.ReduceResonanceTolerance = 1e-8;
    /** Minimal magnitude of the coefficient of a term to take it into account. */
    Chi.CoefficientTolerance = 1e-16;
    /** Minimal magnitude of the coefficient of a term to take it into account with respect to amount of terms. */
    Chi.MultiTermCoefficientTolerance = 1e-6;

    time_temp = clock();
    Chi.prepare();
    std::vector<boost::tuple<ComplexType, ComplexType, ComplexType> > freqs_2pgf;
    Chi.compute(false, freqs_2pgf, world);
    world.barrier();
    print_time(time_temp, "TwoParticleGF.compute");

    time_temp = clock();
    if (!world.rank()) {
        int nm = 2;
        for(int n1 = -nm; n1<nm; ++n1)
            for(int n2 = -nm; n2<nm; ++n2)
                for(int n3 = -nm; n3<nm; ++n3){
                    INFO(n1 << " " << n2 << " " << n3 << "|" << Chi(n1,n2,n3));
                };
        // fprint_Gf2(Chi);
    }
    print_time(time_temp, "TwoParticleGF.getValues");

    print_time(time_start, "Total");
}

void print_section (const std::string& str)
{
    if (!world.rank()) {
        std::cout << std::string(str.size(),'=') << std::endl;
        std::cout << str << std::endl;
        std::cout << std::string(str.size(),'=') << std::endl;
    }
}
