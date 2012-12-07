# All lines beginnig with '|' are Matlab commands,
# so we can simply pipe them into a script 'path.m'
# which will plot the scenario and path.
./cpf  | grep "^|" | cut -f2 -d'|' > path.m
