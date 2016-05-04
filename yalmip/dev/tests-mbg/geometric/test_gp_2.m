function gp

t1 = sdpvar(1,1);
t2 = sdpvar(1,1);
t3 = sdpvar(1,1);
t = [t1 t2 t3];
obj = (40*t1^-1*t2^-0.5*t3^-1)+(20*t1*t3)+(40*t1*t2*t3);
F = set((1/3)*t1^-2*t2^-2+(4/3)*t2^0.5*t3^-1 <= 1);
sol = solvesdp(F + set(t1-t2 <= 1),obj);

mbg_asserttolequal(sol.problem,-4);

