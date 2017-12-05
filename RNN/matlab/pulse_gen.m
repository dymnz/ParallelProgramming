% Generate random sEMG force model %
clear; close all;

% Data length %
NUM_TEST = 10;
DATA_LENGTH = 500;
RAND_THRESHOLD = 1;

% Twitch force param %
P = 1;     % Peak force
CT = 5;    % Time to peak

% File
file_location = '../C/test_data/';
file_name = 'exp10.txt';
fileID = fopen(strcat(file_location, file_name),'w');
fprintf(fileID, '%d\n', NUM_TEST);


for i = 1:NUM_TEST
    RAND_THRESHOLD = rand * 3 + 0.001;
    [pulse_vector, tf_vector, tf_proto] = ...
                random_twitch_force(DATA_LENGTH, RAND_THRESHOLD, P, CT);

    figure;
    subplot_helper(1:DATA_LENGTH, pulse_vector, ...
                    [3 1 1], {'sample' 'amplitude' 'pulse'}, ':x');
    subplot_helper(1:length(tf_proto), tf_proto, ...
                    [3 1 2], {'sample' 'amplitude' 'pulse'}, '-');
    subplot_helper(1:DATA_LENGTH, pulse_vector, ...
                    [3 1 3], {'sample' 'amplitude' 'pulse'}, ':x');   
    subplot_helper(1:DATA_LENGTH, tf_vector, ...
                    [3 1 3], {'sample' 'amplitude' 'pulse'}, '-');  

    fprintf(fileID, '%d %d\n', DATA_LENGTH, 1);
    fprintf(fileID, '%f\t', pulse_vector);
    fprintf(fileID, '\n');
    fprintf(fileID, '%d %d\n', DATA_LENGTH, 1);
    fprintf(fileID, '%f\t', tf_vector);
    fprintf(fileID, '\n');
end

fclose(fileID);