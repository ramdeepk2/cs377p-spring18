[xs, ys] = jacobi();
plot3(xs, ys, 1:25)
title('25 Jacobi Iterations')
xlabel('x values')
ylabel('y values')
zlabel('iteration #')

function [xs, ys] = jacobi()
	m_inverse = [1/3 0; 0 1/2];
	xs = [];
	ys = [];
	x_guess = 0; y_guess = 0;
	for i = 1:25
		new_vals = [x_guess; y_guess] - m_inverse * [(3 * x_guess) - (4 * y_guess) + 1; x_guess + (2 * y_guess) - 3];
		xs(end + 1) = new_vals(1); ys(end + 1) = new_vals(2);
		x_guess = new_vals(1); y_guess = new_vals(2);
    end
end