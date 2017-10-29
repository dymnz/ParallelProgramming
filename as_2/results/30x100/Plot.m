clear; close all;

file_name = '2_8.txt';
title_text = '30sec 8-thread 2opt() call';

file = fileread(file_name);
call_num_strings = regexp(file, '(?<=call:[^0-9]*)[0-9]+', 'match');
call_num_list = str2double(call_num_strings);

call_min = min(call_num_list);
call_max = max(call_num_list);
call_mean = mean(call_num_list);

%%
plot(call_num_list);
xlim([1 100]);
ylim([0.7*call_min 1.2*call_max]);

hold on;
refline(0, mean(call_num_list));
xlabel('test', 'FontSize', 20);
ylabel('2opt\_swap()', 'FontSize', 20);
title(title_text, 'FontSize', 20);

%%
index_min = find(call_min == call_num_list); 
index_max = find(call_max == call_num_list); 

strmin = [num2str(call_min)];
text(index_min, call_min,strmin, 'HorizontalAlignment', 'left', 'FontSize', 15);

strmax = [num2str(call_max)];
text(index_max, call_max, strmax, 'HorizontalAlignment', 'right', 'FontSize', 15);

stravg = [num2str(call_mean)];
text(length(call_num_list), call_mean, stravg, 'HorizontalAlignment', 'right', 'FontSize', 15);