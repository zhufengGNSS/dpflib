figure

fps=1;
mov = struct('cdata',[],'colormap',[]);
set(gca,'nextplot','replacechildren');

hold on

for t=1:T
	% draw base (sensors, etc)
	clf
	hold on
	xlim([0.000000,3.200000]);
	ylim([0.000000,6.000000]);
	plot(l(:,1),l(:,2),'k-','LineWidth',14);
	[J d] = size(s);
	for j=1:J
		plot(s(j,1),s(j,2),'ks','MarkerSize',8,'LineWidth',2);
		if y(t,j)
			plot([l(1,1),s(j,1)],[l(1,2),s(j,2)],'k:','LineWidth',1)
			plot([l(2,1),s(j,1)],[l(2,2),s(j,2)],'k:','LineWidth',1)
		 end
	end
	% draw sensors
	plot(x_pos(t,1),x_pos(t,2),'kx','MarkerSize',8,'LineWidth',2);
	plot(x_est(t,1),x_est(t,2),'rx','MarkerSize',8,'LineWidth',2);
	for i=1:I
		 plot(x(t,1,i),x(t,2,i),'r*','MarkerSize',4);
	end
	%M(t) = getframe;
	%f = getframe;
	mov(t) = getframe(gcf);
	hold off;
	pause(0.1)
end
movie2avi(mov,'cpf.avi','compression','None','fps',1);

