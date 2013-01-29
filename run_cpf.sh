# All lines beginnig with '|' are Matlab commands,
# so we can simply pipe them into a script 'path.m'
# which will plot the scenario and path.
for ((i=0;i<10;i++)); do
	f="plot_cpf$i.m"
	./cpf $i | grep "^|" | cut -f2 -d'|' > data/$f
	cat matlab/plot.m >> data/$f
done
