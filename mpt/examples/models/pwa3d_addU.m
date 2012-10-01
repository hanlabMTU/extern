%PWA3D_ADDU 3rd order PWA example with 2 PWA dynamics and an additive uncertainty
%
% ---------------------------------------------------------------------------
% DESCRIPTION
% ---------------------------------------------------------------------------
%
% The example taken from:
% 
% author = {D.Q. Mayne and S. Rakovi\'{c}},
% year = 2003,
% title = {Model predicitve control of constrained piecewise affine discrete-time systems},
% journal = {Int. J. of Robust and Nonlinear Control},
% volume = 13,
% number=3,
% month=apr,
% pages = {261--279}
%
%
% Default values are:
%   * Prediction horizon N = Inf
%   * Weight in the cost function Q = I, R = 0.1
%   * No bounds on x0
%   * Level of suboptimality = 1   (needed for PWA systems!)
%   * Additive uncertainty noise = unitbox(3,0.1)
%
% USAGE:
%   pwa3d_addU
%   [ctrlStruct] = mpt_control(sysStruct,probStruct);
%
% ---------------------------------------------------------------------------
% INPUT
% ---------------------------------------------------------------------------
% none
%
% ---------------------------------------------------------------------------
% OUTPUT                                                                                                    
% ---------------------------------------------------------------------------
% sysStruct, probStruct - system and problem definition structures stores
%                         in the workspace
%

% Copyright is with the following author(s):
%
% (C) 2003 Michal Kvasnica, Automatic Control Laboratory, ETH Zurich,
%          kvasnica@control.ee.ethz.ch
% (C) 2003 Pascal Grieder, Automatic Control Laboratory, ETH Zurich,
%          grieder@control.ee.ethz.ch

% ---------------------------------------------------------------------------
% Legal note:
%          This program is free software; you can redistribute it and/or
%          modify it under the terms of the GNU General Public
%          License as published by the Free Software Foundation; either
%          version 2.1 of the License, or (at your option) any later version.
%
%          This program is distributed in the hope that it will be useful,
%          but WITHOUT ANY WARRANTY; without even the implied warranty of
%          MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
%          General Public License for more details.
% 
%          You should have received a copy of the GNU General Public
%          License along with this library; if not, write to the 
%          Free Software Foundation, Inc., 
%          59 Temple Place, Suite 330, 
%          Boston, MA  02111-1307  USA
%
% ---------------------------------------------------------------------------


clear sysStruct probStruct

H=[eye(3);-eye(3)];
K=[5;3;5;5;3;5]; 

% PWA description 1: x(k+1)=A{1} x(k) + B{1} u(k) + f{1}
sysStruct.A{1} = [1 0.5 0.3; 0 1 1;0 0 1];
sysStruct.B{1} = [0; 0; 1];
sysStruct.f{1} = [0; 0; 0];
% y(k) = C{1} x(k) + D{1} u(k) + g{1}
sysStruct.C{1} = eye(3);
sysStruct.D{1} = zeros(3,1);
sysStruct.g{1} = zeros(3,1);
% guardX{1}*x(k)+guardU{1}*u(k)<=guardC{1}
sysStruct.guardX{1}=[0 1 0; H];
sysStruct.guardC{1}=[    1; K];

% PWA description 2: x(k+1)=A{2} x(k) + B{2} u(k) + f{2}
sysStruct.A{2} = [1 0.2 0.3; 0 0.5 1;0 0 1];
sysStruct.B{2} = [0; 0; 1];
sysStruct.f{2} = [0.3; 0.5; 0];
% y(k) = C{2} x(k) + D{2} u(k) + g{2}
sysStruct.C{2} = eye(3);
sysStruct.D{2} = zeros(3,1);
sysStruct.g{2} = zeros(3,1);
% guardX{2}*x(k)+guardU{2}*u(k)<=guardC{2}
sysStruct.guardX{2}=[0 -1 0; H];
sysStruct.guardC{2}=[    -1; K];

sysStruct.ymin=[-5;-3;-5];
sysStruct.ymax=[5; 3; 5];
sysStruct.umin = -1;
sysStruct.umax = 1;


probStruct.Q  = 1*eye(3);  % weight on the state (nx x nx)
probStruct.R  = 0.1;         % weight on the input (nu x nu)
probStruct.N = Inf;        % prediction horizon
probStruct.norm = 2;
probStruct.subopt_lev=1;
probStruct.x0bounds=0;

sysStruct.noise=polytope([eye(3); -eye(3)],[1;1;1;1;1;1]*0.1);