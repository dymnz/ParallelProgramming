% Generate random sEMG force model %
clear; close all;

% Data length %
NUM_TEST = 2;
DATA_LENGTH = 100;
RAND_THRESHOLD = 1;

% Twitch force param %
P = 1;     % Peak force
CT = 5;    % Time to peak


% File
file_location = '../C/test_data/';
file_name = 'exp.txt';
fileID = fopen(strcat(file_location, file_name),'w');
fprintf(fileID, '%d\n', NUM_TEST);


for i = 1:NUM_TEST
rand_vector = normrnd(0, 1, DATA_LENGTH, 1);
pulse_vector = zeros(DATA_LENGTH, 1);

pulse_vector(rand_vector>RAND_THRESHOLD) = ...
    abs(                                                ...
        rand_vector(1:length(find(rand_vector>RAND_THRESHOLD))) .*   ...
        rand_vector(rand_vector>RAND_THRESHOLD));

pulse_vector = pulse_vector ./ max(pulse_vector);    
    
tf_proto = twitch_force(1:CT*5, P, CT);

% Copy pasting
tf_vector = conv(pulse_vector, tf_proto);

% Truncation
tf_vector = tf_vector(1:DATA_LENGTH);

% Normalization
tf_vector = tf_vector ./ max(tf_vector);


figure;
subplot_helper(1:DATA_LENGTH, pulse_vector, ...
                [3 1 1], {'sample' 'amplitude' 'pulse'});
subplot_helper(1:length(tf_proto), tf_proto, ...
                [3 1 2], {'sample' 'amplitude' 'pulse'});
subplot_helper(1:DATA_LENGTH, pulse_vector, ...
                [3 1 3], {'sample' 'amplitude' 'pulse'});   
subplot_helper(1:DATA_LENGTH, tf_vector, ...
                [3 1 3], {'sample' 'amplitude' 'pulse'});  
            
fprintf(fileID, '%d %d\n', DATA_LENGTH, 1);
fprintf(fileID, '%f\t', pulse_vector);
fprintf(fileID, '\n');
fprintf(fileID, '%d %d\n', DATA_LENGTH, 1);
fprintf(fileID, '%f\t', tf_vector);
fprintf(fileID, '\n');
end