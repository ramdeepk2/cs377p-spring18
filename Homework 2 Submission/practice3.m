times = 0.025:0.025:(10 - 0.025);
f_hat = zeros(400, 41);
value_row = zeros(1, 41);

% create first row
for i = 2:40
	value_row(i) = exp(-4 * ((((i - 1) / 4) - 5)^2));
end
value_row(41) = 0;
value_row;
f_hat = [value_row; f_hat];
 
for j = 2:401
	for i = 39:-1:2
		f_hat(j, i) = f_hat(j - 1, i) + 0.4 * (f_hat(j - 1, i + 1) - (2 * f_hat(j - 1, i)) + f_hat(j - 1, i - 1));
	end
end
 
f_hat;

% after matrix constructed, transpose it to get real f_hat
real_f_hat = f_hat.';
hold on
for tt = 1:401
    plot(0:.25:10, real_f_hat(:,tt))
end
xlabel("Position")
ylabel("Temperature")