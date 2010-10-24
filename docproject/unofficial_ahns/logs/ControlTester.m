% Flight Control Tester
%
% Tim Molloy
% 21 October 2010
clear all
close all

mcuFile = fopen('ControlTester/mcu.log','r');
mcuUpdate = fscanf(mcuFile,'%f');
controlFile = fopen('ControlTester/control.log','r');
controlUpdate = fscanf(controlFile,'%lf');
controlUpdate(1:4) = 0;

figure(1)
plot(mcuUpdate)
hold on
plot(1:length(mcuUpdate),mean(mcuUpdate),'g')
hold off
grid on
title('MCU Thread Update Rate')
xlabel('Update Count')
ylabel('Update Rate [Hz]')
legend({'Instant Rate','Average Rate'})

figure(2)
plot(controlUpdate)
hold on
plot(1:length(controlUpdate),mean(controlUpdate),'g')
hold off
grid on
title('Control Thread Update Rate')
xlabel('Update Count')
ylabel('Update Rate [Hz]')
legend({'Instant Rate','Average Rate'})

fclose(mcuFile);
fclose(controlFile);