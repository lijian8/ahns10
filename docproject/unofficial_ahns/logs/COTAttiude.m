%% COT Altitude Data
% October 18 2010
% Tim Molloy
% Flight Test Files
clear all
close all

%% Rig Step Cases 
date = 'Monday-30-08-2010-194506.log';
resultRanges = [225 260 -50 100; 283 320 -50 50; 420 500 -10 45; 500 640 -10 25; 740 850 -45 15];
titles = {'RC Step Command','Step of 20^\circ','Return after Step','Smaller Step Disturbances','Smaller Step Disturbances'};

%% Filtered State Data
% TIME, F_PHI, F_PHI_DOT, F_THETA, F_THETA_DOT, F_PSI, F_PSI_DOT, F_X,
% F_X_DOT, F_AX, F_Y, F_Y_DOT, F_AY, F_Z, F_Z_DOT, F_AZ, VOLTAGE
readData = csvread(['rack_31082010/Filtered_States_' date],2,0);

t = readData(:,1);
phi = readData(:,2);
p = readData(:,3);
theta = readData(:,4);
q = readData(:,5);
psi = readData(:,6);
r = readData(:,7);
x = readData(:,8);
vx = readData(:,9);
ax = readData(:,10);
y = readData(:,11);
vy = readData(:,12);
ay = readData(:,13);
z = readData(:,14);
vz = readData(:,15);
az = readData(:,16);
voltage = readData(:,17);

%% Autopilot Reference
% TIME, REF_PHI, REF_THETA, REF_PSI, REF_X, REF_Y, REF_Z, PHI_ACTIVE,
% THETA_ACTIVE, PSI_ACTIVE, X_ACTIVE, Y_ACTIVE, Z_ACTIVE
readData = csvread(['rack_31082010/AP_State_' date],2,0);

t_ap = readData(:,1);
ref_phi = readData(:,2);
ref_theta = readData(:,3);
ref_psi = readData(:,4);
ref_x = readData(:,5);
ref_y = readData(:,6);
ref_z = readData(:,7);
active_phi = readData(:,8);
active_theta = readData(:,9);
active_psi = readData(:,10);
active_x = readData(:,11);
active_y = readData(:,12);
active_z = readData(:,13);


%% Plot Results
for ii = 1:size(resultRanges,1)
    figure(ii)
    plot(t,rad2deg(theta),'--',t_ap,rad2deg(ref_theta),'LineWidth',2)
    grid on;
    title(['Pitch Control In Test Rig : ' titles{ii}])
    legend({'\theta','\theta_{ref}'},'Orientation','Horizontal','Location','NorthEast')
    ylabel('Pitch Angle \theta [deg]')
    xlabel('Time t [sec]')
    axis(resultRanges(ii,:))
end
