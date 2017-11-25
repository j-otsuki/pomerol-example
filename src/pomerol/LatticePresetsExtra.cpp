#include "pomerol/LatticePresetsExtra.h"
#include "external/AtomicCoefficients.h"

namespace Pomerol {

//
// Lattice::Term::Presets
//

Lattice::Term* Lattice_Term_Presets::FourOrbitals ( const std::string& Label, MelemType Value, unsigned short orbital1, unsigned short orbital2, unsigned short orbital3, unsigned short orbital4, unsigned short spin1, unsigned short spin2 )
{
    Lattice::Term *T = new Lattice::Term(4);
    bool order[4]              =      { 1,     1,     0,     0     };
    unsigned short Spins[4]    =      { spin1, spin2, spin2, spin1 };
    std::string Labels[4]      =      { Label, Label, Label, Label };
    unsigned short Orbitals[4] =      { orbital1, orbital2, orbital4, orbital3 };
    T->OperatorSequence.assign(order,order+4);
    T->SiteLabels.assign(Labels,Labels+4);
    T->Orbitals.assign(Orbitals,Orbitals+4);
    T->Spins.assign(Spins,Spins+4);
    T->Value = Value;
    return T;
}

Lattice::Term* Lattice_Term_Presets::SpinOrbit ( const std::string& Label, MelemType Value, unsigned short orbital1, unsigned short orbital2, unsigned short spin1, unsigned short spin2)
{
    Lattice::Term *T = new Lattice::Term(2);
    bool OperatorSequence[2] = { 1, 0 };
    std::string Labels[2] = { Label, Label };
    unsigned short Orbitals[2] = {orbital1, orbital2 };
    unsigned short Spins[2] = { spin1, spin2 };
    T->OperatorSequence.assign(OperatorSequence,OperatorSequence+2);
    T->SiteLabels.assign(Labels,Labels+2);
    T->Orbitals.assign(Orbitals,Orbitals+2);
    T->Spins.assign(Spins,Spins+2);
    T->Value = Value;
    return T;
};

//
// LatticePresets
//

void LatticePresetsExtra::addCoulombF(Lattice *L, const std::string& Label, std::vector<MelemType> &Fk, int l, MelemType Level)
{
    if( Fk.size() < l+1 ) { ERROR("Fk.size must be >= l+1"); throw (Lattice::exWrongLabel()); };
    Lattice::SiteMap Sites=L->getSiteMap();
    // unsigned short Orbitals = L->Sites[Label]->OrbitalSize;
    // unsigned short Orbitals = L->getSite(Label).OrbitalSize; // this does not work
    unsigned short Orbitals = Sites[Label]->OrbitalSize;
    // unsigned short Spins = L->Sites[Label]->SpinSize;
    // unsigned short Spins = L->getSite(Label).SpinSize;
    unsigned short Spins = Sites[Label]->SpinSize;
    if( Orbitals != 2*l+1 ) { ERROR("Num of orbitals must be 2l+1"); throw (Lattice::exWrongLabel()); };
    if( Spins != 2 ) { ERROR("Spin must be 1/2"); throw (Lattice::exWrongLabel()); };
    for(int i1=0; i1<Orbitals; i1++){
        for(int i2=0; i2<Orbitals; i2++){
            for(int i3=0; i3<Orbitals; i3++){
                for(int i4=0; i4<Orbitals; i4++){
                    // m = i-l (quantum number of L_z)
                    double v = atomic_u(l, Fk, i1-l, i2-l, i3-l, i4-l) / 2.;
                    if( std::abs(v) > 1e-14 ){  // if non-zero (drop rounding error)
                        for (unsigned short z1=0; z1<Spins; ++z1){
                            for (unsigned short z2=0; z2<Spins; ++z2){
                                // L->Terms->addTerm(Lattice::Term::Presets::FourOrbitals(Label, v, i1, i2, i3, i4, z1, z2));
                                // L->getTermStorage().addTerm(Lattice_Term_Presets::FourOrbitals(Label, v, i1, i2, i3, i4, z1, z2));
                                L->addTerm(Lattice_Term_Presets::FourOrbitals(Label, v, i1, i2, i3, i4, z1, z2));
                            }
                        }
                    }
                }
            }
        }
    }
    for (unsigned short i=0; i<Orbitals; ++i){
        for (unsigned short z=0; z<Spins; ++z){
            // if (std::abs(Level)) L->Terms->addTerm(Lattice::Term::Presets::Level(Label, Level, i, z ));
            if (std::abs(Level)) L->addTerm(Lattice::Term::Presets::Level(Label, Level, i, z ));
        }
    }
}

void LatticePresetsExtra::addSpinOrbit ( Lattice *L, const std::string& Label, int l, MelemType lambda)
{
    if( l==0 )  return;
    Lattice::SiteMap Sites=L->getSiteMap();
    // unsigned short Orbitals = L->Sites[Label]->OrbitalSize;
    // unsigned short Orbitals = L->getSite(Label).OrbitalSize; // this does not work
    unsigned short Orbitals = Sites[Label]->OrbitalSize;
    // unsigned short Spins = L->Sites[Label]->SpinSize;
    // unsigned short Spins = L->getSite(Label).SpinSize;
    unsigned short Spins = Sites[Label]->SpinSize;
    if( Orbitals != 2*l+1 ) { ERROR("Num of orbitals must be 2l+1"); throw (Lattice::exWrongLabel()); };
    if( Spins != 2 ) { ERROR("Spin must be 1/2"); throw (Lattice::exWrongLabel()); };
    for(int i1=0; i1<Orbitals; i1++){
        for(int i2=0; i2<Orbitals; i2++){
            for (int z1=0; z1<Spins; z1++){
                for (int z2=0; z2<Spins; z2++){
                    // m = i-l (quantum number of L_z)
                    // 2*sz = 2*z-1 (quantum number of 2*S_z)
                    double x = coef_ls(l, i1-l, 2*z1-1, i2-l, 2*z2-1) * lambda;
                    if( std::abs(x) > 1e-14 ){  // if non-zero (drop rounding error)
                        // L->Terms->addTerm(Lattice::Term::Presets::SpinOrbit(Label, x, i1, i2, z1, z2));
                        L->addTerm(Lattice_Term_Presets::SpinOrbit(Label, x, i1, i2, z1, z2));
                    }
                }
            }
        }
    }
}

} // end of namespace Pomerol
