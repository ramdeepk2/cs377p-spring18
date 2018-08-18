[xs, ys] = gauss_seidel();
plot3(xs, ys, 1:25);
title('25 Gauss-Seidel Iterations');
xlabel('x guesses');
ylabel('y guesses');
zlabel('iteration #');
% The major difference beween gauss-seidel and jacobi is that jacobi keeps
% iteration i + 1 and iteration i completely separate. Gauss-seidel uses the
% estimated value of x_i+1 to estimate the value of y_i+1
function [xs, ys] = gauss_seidel()
	x_guess = 0; y_guess = 0;
	xs = []; ys = [];
	for i = 1:25
		x_guess = (-1 + (4 * y_guess)) / 3;
		xs(end + 1) = x_guess;
		y_guess = (3 - x_guess) / 2;
		ys(end + 1) = y_guess;
	end
end