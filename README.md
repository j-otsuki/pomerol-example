### Atomic states with rotationally invariant Coulomb interactions

#### How to use:
```
cmake -Dpomerol_DIR=/where_pomerol_is_installed/share/pomerol /path_to_this_directory
make
./atomic
gnuplot /path_to_this_directory/*.plt  # generate pdfs
```

#### Parameters
```
l_orb        : angular momentum
Fk[l_orb+1]  : Slater integrals (F_0=U, F_2, ..., F_{2*l_orb}
mu           : chemical potential
lambda_ls    : Spin-orbit coupling
```
