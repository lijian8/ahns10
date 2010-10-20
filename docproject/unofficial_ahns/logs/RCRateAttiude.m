%% Free Angluar Rates Altitude Data
% October 20 2010
% Tim Molloy
% Flight Test Files
clear all
close all

%% Demonstration Tests
date = 'Friday-15-10-2010-181925.log';
resultRanges = [1280 1302; 1312 1334; 1365 1375; 1393 1407; 1069 1091; 988 1020; 628 647; 414 439];
angleRanges = [-50 50; -30 30; -30 30; -30 30; -50 50; -50 50; -50 50; -50 50];
rateRanges = [-200 200; -200 200; -200 200; -200 200; -200 200; -200 200; -200 200; -200 200];
titles = {'MVI0048','MVI0049','MVI0050','MVI0051','MVI0047','MVI0046','MVI0045','MVI0042'};

%% Filtered State Data
% TIME, F_PHI, F_PHI_DOT, F_THETA, F_THETA_DOT, F_PSI, F_PSI_DOT, F_X,
% F_X_DOT, F_AX, F_Y, F_Y_DOT, F_AY, F_Z, F_Z_DOT, F_AZ, VOLTAGE
readData = csvread(['Tims House 2/Attitude Final logs/Filtered_States_' date],2,0);

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
readData = csvread(['Tims House 2/Attitude Final logs/AP_State_' date],2,0);

t_ap = readData(:,1);
ref_phi = readData(:,2);
ref_p = ((ref_phi - 1000)/(2000-1000)*(300--300)-300);
ref_theta = readData(:,3);
ref_q =((ref_theta - 1000)/(2000-1000)*(300--300)-300);
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

%% Flight Computer Log File
% TIME, COMMANDED_ENGINE1, COMMANDED_ENGINE2, COMMANDED_ENGINE3,
% COMMANDED_ENGINE4, RC_LINK, FC_UPTIME, FC_CPU
readData = csvread(['Tims House 2/Attitude Final logs/FC_States_' date],2,0);
t_fc = readData(:,1);
rcThrottle = readData(:,2);
rcRoll = readData(:,3);
rcPitch = readData(:,4);
rcYaw = readData(:,5);
rcMode = readData(:,6);

%% Plot Results
kk = 1;
for ii = 1:2:2*size(resultRanges,1)
    figure(ii)
    subplot(2,1,1)
    plot(t,rad2deg(q),t_ap,(ref_q),'LineWidth',2)
    grid on;
    title(['Pitch Control : ' titles{kk}])
    legend({'q','q_{ref}'},'Orientation','Horizontal','Location','NorthEast')
    ylabel('Pitch Angle Rate q [deg/s]')
    axis([resultRanges(kk,:) rateRanges(kk,:)])
    
    subplot(2,1,2)
    plot(t,rad2deg(theta))
    grid on;
    legend({'\theta'},'Orientation','Horizontal','Location','NorthEast')
    ylabel('Pitch Angle \theta [deg]')
    xlabel('Time t [sec]')
    axis([resultRanges(kk,:) angleRanges(kk,:)])
    
    figure(ii+1)
    subplot(2,1,1)
    plot(t,rad2deg(p),t_ap,(ref_p),'LineWidth',2)
    title(['Roll Control : ' titles{kk}])
    grid on;
    legend({'p','p_{ref}'},'Orientation','Horizontal','Location','NorthEast')
    ylabel('Roll Angle Rate p [deg/s]')
    xlabel('Time t [sec]')
    axis([resultRanges(kk,:) rateRanges(kk,:)])
    
    subplot(2,1,2)
    plot(t,rad2deg(phi))
    grid on;
    legend({'\phi'},'Orientation','Horizontal','Location','NorthEast')
    ylabel('Roll Angle \phi [deg]')
    xlabel('Time t [sec]')
    axis([resultRanges(kk,:) angleRanges(kk,:)])
    kk = kk + 1;
end
