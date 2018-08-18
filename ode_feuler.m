ys = ode_center();
ys2 = ode();
% scatter(0:.1:2, ys);
% title("Centered Euler Method for y' = y * sin(x)")
% xlabel('x = ih')
% ylabel('y(ih)')
function ys = ode()
    h = 0.1;
    ys = [1];
    for i = 1:20
        y_iplusone = ys(end) * ((h * sin(i * h)) + 1);
        ys(end + 1) = y_iplusone;
    end
end

function ys = ode_center()
    h = 0.1;
    ys = [1 1];
    for i = 1:19
        y_iplusone = (2 * h * ys(end) * sin(i * h)) + ys(end - 1);
        ys(end + 1) = y_iplusone;
    end
end