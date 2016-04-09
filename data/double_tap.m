filename = 'double_tap3.csv'
data = csvread(filename);
data = data([1:250],:);

meann = mean(data);
stdd = std(data);
I = bsxfun(@lt, meann + 2*stdd, data) | bsxfun(@gt, meann - 2*stdd, data)

figure % new figure
ax1 = subplot(3,1,1); % top subplot
ax2 = subplot(3,1,2); % middle subplot
ax3 = subplot(3,1,3); % bottom subplot

plot(ax1,data);  
plot(ax2,I);
plot(ax3,abs(data-meann));

disp(meann);
disp(stdd);