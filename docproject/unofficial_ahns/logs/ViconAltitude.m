%% Ultrasonic Altitude Data
% October 18 2010
% Tim Molloy
% Flight Test Files
clear all
close all
clc

%% Comment In and Out the following to give different logs

%% Best Tests
date = 'Friday-01-10-2010-144806.log';
resultRanges = [615 660; 720 780; 1450 1505; 1640 1700]; 
labels = {'First, Kp Only','Second, Increased Kd', 'Manual Trim','Flight with Discharged Battery and Manual Trim'};

%% Better Tests and Accumulator
% date = 'Friday-01-10-2010-163330.log';
% resultRanges = [95 120; 135 155; 170 185; 275 330; 395 410];
% labels = {'Charged Battery','1 Flight Discharge, Increased Kd','2 Flight Discharges','Manual Trim','Accumulator'};

%% Final Accumulator
%date = 'Wednesday-06-10-2010-071053.log';
%resultRanges = [105 177; 476 527; 803 808];
%labels = {'Manual Accumulator','Slow Accumulator','Accumulator Drop - AP Turned off'};


%% Filtered State Data
% TIME, F_PHI, F_PHI_DOT, F_THETA, F_THETA_DOT, F_PSI, F_PSI_DOT, F_X,
% F_X_DOT, F_AX, F_Y, F_Y_DOT, F_AY, F_Z, F_Z_DOT, F_AZ, VOLTAGE
readData = csvread(['Vicon Vertical Working/Filtered_States_' date],2,0);

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

%% Vicon Data
% TIME, VICON_X, VICON_Y, VICON_Z, VICON_VX, VICON_VY, VICON_VZ, VICON_PHI, VICON_THETA, VICON_PSI
readData = csvread(['Vicon Vertical Working/Vicon_' date],2,0);

t_vi = readData(:,1);
x_vi = readData(:,2);
y_vi = readData(:,3);
z_vi = readData(:,4);
vx_vi = readData(:,5);
vy_vi = readData(:,6);
vz_vi = readData(:,7);
phi_vi = readData(:,8);
theta_vi = readData(:,9);
psi_vi = readData(:,10);

%% Autopilot Reference
% TIME, REF_PHI, REF_THETA, REF_PSI, REF_X, REF_Y, REF_Z, PHI_ACTIVE,
% THETA_ACTIVE, PSI_ACTIVE, X_ACTIVE, Y_ACTIVE, Z_ACTIVE

readData = csvread(['Vicon Vertical Working/AP_State_' date],2,0);

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

%% Flight Computer Log File
% TIME, COMMANDED_ENGINE1, COMMANDED_ENGINE2, COMMANDED_ENGINE3,
% COMMANDED_ENGINE4, RC_LINK, FC_UPTIME, FC_CPU
readData = csvread(['Vicon Vertical Working/FC_States_' date],2,0);
t_fc = readData(:,1);
rcThrottle = readData(:,2);
rcRoll = readData(:,3);
rcPitch = readData(:,4);
rcYaw = readData(:,5);
rcMode = readData(:,6);


%% Plot Results
for ii = 1:size(resultRanges,1)
    figure(ii)
    %subplot(2,1,1)
    plot(t_vi,z_vi,t_ap,ref_z)
    grid on;
    ylabel('Altitude z, [m]')
    tempAxis = axis;
    tempAxis(1:2) = resultRanges(ii,:);
    axis(tempAxis)
    title(['Altitude with Vicon : ' labels{ii}])
    legend({'z','z_{ref}'},'Orientation','Horizontal','Location','Best')
    
%     subplot(2,1,2)
%     plot(t_vi,vz_vi,'--')
%     grid on;
%     ylabel('Velocity z [m/s]')
%     tempAxis = axis;
%     tempAxis(1:2) = resultRanges(ii,:);
%     axis(tempAxis)
end
