fig = 800;

hour = data(:,4);
minute = data(:,5);
second = data(:,6);
millisecond = data(:,7);

millisec = hour*3.6e6 + minute*6e4 + second*1e3 + millisecond;
deci_hour = millisec/3.6e6;

altitude = data(:,14)/100;


alt_cmd = data(:,8);
alt_p = data(:,21);
alt_i = data(:,22);
alt_p_gain = data(:,34);
alt_i_gain = data(:,35);
collective = data(:,24);

len = 100;
theFilter = ones(1,len)/len;
filtd_alt = conv(altitude, theFilter);
filtd_col = conv(collective, theFilter);
filtd_altp = conv(alt_p, theFilter);
filtd_alt = filtd_alt(1:end-(len-1));
filtd_col = filtd_col(1:end-(len-1));
filtd_altp = filtd_altp(1:end-(len-1));

stepsize = alt_cmd([ end 1:end-1 ]) - alt_cmd;

fig = fig+1;
figure(fig)
plot(deci_hour,filtd_alt,'b')
hold on
stem(deci_hour,5000*abs(stepsize),'r.')
plot(deci_hour,filtd_altp/20+1.5,'c:')
plot(deci_hour,alt_i/20+1.5,'k:','linewidth',1)
plot(deci_hour,alt_p_gain,'m','linewidth',2)
plot(deci_hour,alt_cmd,'r','linewidth',2)
plot(deci_hour,filtd_col/8+1.5,'g','linewidth',1)
hold off
axis([23.726, 23.74, 0.75, 2.2])
legend('altitude', 'step', 'alt_p', 'alt_i', 'alt_pgain', 'alt cmd', 'collective')
