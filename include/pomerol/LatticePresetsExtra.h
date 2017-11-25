/** \file include/pomerol/LatticePresets.h
** \brief A set of preset methods to simplify Pomerol::Lattice entering.
**
** \author Andrey Antipov (Andrey.E.Antipov@gmail.com)
** \author Igor Krivenko (Igor.S.Krivenko@gmail.com)
*/

#ifndef __INCLUDE_LATTICE_SITES_PRESETS_EXTRA_H
#define __INCLUDE_LATTICE_SITES_PRESETS_EXTRA_H

// #include "Misc.h"
// #include "Lattice.h"
#include <pomerol/LatticePresets.h>

namespace Pomerol{

/** This is a set of presets of different Terms, most commonly used while writing a Hamiltonian. */
// class Lattice::Term::PresetsExtra{
// class Lattice::Term::PresetsExtra : public Lattice::Term::Presets{
// public:
//     static Term* FourOrbitals ( const std::string& Label, MelemType Value, unsigned short orbital1, unsigned short orbital2, unsigned short orbital3, unsigned short orbital4, unsigned short spin1, unsigned short spin2);
//
//     static Term* SpinOrbit ( const std::string& Label, MelemType Value, unsigned short orbital1, unsigned short orbital2, unsigned short spin1, unsigned short spin2);
// };
class Lattice_Term_Presets {
public:
    static Lattice::Term* FourOrbitals ( const std::string& Label, MelemType Value, unsigned short orbital1, unsigned short orbital2, unsigned short orbital3, unsigned short orbital4, unsigned short spin1, unsigned short spin2);

    static Lattice::Term* SpinOrbit ( const std::string& Label, MelemType Value, unsigned short orbital1, unsigned short orbital2, unsigned short spin1, unsigned short spin2);
};

/** A set of presets to fill the Lattice::TermStorage and Lattice::Sites for some commonly used examples. */
// class LatticePresets : LatticePresets{
class LatticePresetsExtra : public LatticePresets {
public:
    static void addCoulombF(Lattice *L, const std::string& label, std::vector<MelemType> &Fk, int l, MelemType Level);

    static void addSpinOrbit ( Lattice *L, const std::string& Label, int l, MelemType lambda);
};

}; // end of namespace Pomerol

#endif // endif : ifndef __INCLUDE_LATTICE_SITES_PRESETS_H
