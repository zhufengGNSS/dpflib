hold on;
box on;
grid on;
xlim([MIN_X,LIM_X]);
ylim([MIN_Y,LIM_Y]);
plot(l(:,1),l(:,2),'k-','LineWidth',14);
[J d] = size(s);
for j=1:J
    plot(s(j,1),s(j,2),'ks','MarkerSize',8,'LineWidth',2)
end
plot(x_pos(:,1),x_pos(:,2),'-ko','MarkerSize',4,'LineWidth',2);
plot(x_est(:,1),x_est(:,2),'ro-','MarkerSize',4,'LineWidth',2)
hold off
