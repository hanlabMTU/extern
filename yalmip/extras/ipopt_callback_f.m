function f = ipopt_callback_f(x,model)

global latest_x_f
global latest_df

x = x(:);
[f,latest_df] = fmincon_fun(x,model);
latest_x_f = x;