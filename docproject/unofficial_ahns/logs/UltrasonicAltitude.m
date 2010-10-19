%% Ultrasonic Altitude Data
% October 18 2010
% Tim Molloy
% Flight Test Files


%% Comment In and Out the following to give different logs

%% Tests using no D Gain
 date = 'Friday-15-10-2010-141305.log';
 resultRanges = [116 140; 200 255; 295 315; 435 470; 486 520]; 

%% Tests using Vicon Gains
%date = 'Friday-15-10-2010-142953.log';
%resultRanges = [105 160; 175 210]; 

%% Filtered State Data
% TIME, F_PHI, F_PHI_DOT, F_THETA, F_THETA_DOT, F_PSI, F_PSI_DOT, F_X,
% F_X_DOT, F_AX, F_Y, F_Y_DOT, F_AY, F_Z, F_Z_DOT, F_AZ, VOLTAGE
readData = csvread(['Tims House 2/Altitude Ultrasonic/Filtered_States_' date],2,0);

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

readData = csvread(['Tims House 2/Altitude Ultrasonic/AP_State_' date],2,0);
h = 1;

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
readData = csvread(['Tims House 2/Altitude Ultrasonic/FC_States_' date],2,0);
t_fc = readData(:,1);
rcThrottle = readData(:,2);
rcRoll = readData(:,3);
rcPitch = readData(:,4);
rcYaw = readData(:,5);
rcMode = readData(:,6);


%% Plot Results
for ii = 1:size(resultRanges,1)
    figure(ii)
    subplot(3,1,1)
    plot(t,z,t_ap,ref_z)
    grid on;
    ylabel('Altitude z, [m]')
    tempAxis = axis;
    tempAxis(1:2) = resultRanges(ii,:);
    axis(tempAxis)
    title(['Altitude with Ultrasonic Sensor'])
    legend({'z','z_{ref}'},'Orientation','Horizontal','Location','Best')
    
    subplot(3,1,2)
    plot(t,vz,'--')
    grid on;
    ylabel('Velocity z [m/s]')
    tempAxis = axis;
    tempAxis(1:2) = resultRanges(ii,:);
    axis(tempAxis)
    
    subplot(3,1,3)
    plot(t_fc,rcThrottle)
    grid on
    ylabel('Throttle Input U_1 [\mus]')
    tempAxis = axis;
    tempAxis(1:2) = resultRanges(ii,:);
    axis(tempAxis)
    xlabel('Time, sec')
end
