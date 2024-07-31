function [meeting_points,path] = apf_3d_not_in_form(start ,goal, map, obstacles, options)
%APF_3D Computes path between start and goal, according to the idea
% proposed in [Khatib, O. (1986). Real-Time Obstacle Avoidance for
% Manipulators and Mobile Robots. In: Cox, I.J., Wilfong, G.T. (eds) 
% Autonomous Robot Vehicles. Springer, New York, NY. 
% https://doi.org/10.1007/978-1-4613-8997-2_29]. 

%    %%TODO: modificare descrizione + aggiungere commenti

%    An attraction force is applied between the current position and the
%    goal, with coefficient rho and according to the formula provided in
%    func_attr.
%    A repulsion force is applied to the point from the surrounding
%    terrain if the distance is less than r_inf_terrain, with coefficient
%    rho_terrain and according to the formula provided in func_terrain.
%    Finally, a repulsion force is applied to the point by every obstacle
%    closer than r_inf_obstacles, with coefficient rho_obstacles and
%    according to the formula provided in func_obstacles.

arguments
    start (:,3)
    goal (1,3)
    map (:,:) double {mustBeNonnegative, mustBeInteger};
    obstacles (1,:) Obstacle;
    options.rho (1,1) double = 0.0;                                                                                                                         % Initial Coefficient of the gravitational potential field of the Goal 
    options.min_multiplier (1,1) double = 0.01;                                                                                                             % Mininum multiplier for the drones attraction force 
    options.max_multiplier (1,1) double = 7;                                                                                                                % Maximum multiplier for the drones attraction force
    options.rho_drones (1,1) double = 0.5;                                                                                                 % Coefficient of the gravitational potential field of the Drones
    options.rho_terrain (1,1) double {mustBePositive} = 0.7;                                                                                                % Coefficient of the repulsive force field from the terrain
    options.rho_obstacles (1,1) double {mustBePositive} = 5;                                                                                                % Coefficient of the repulsive force field from the obstacles
    options.func_attr {mustBeFunction} = @(rho, p, g, d) rho*(g-p)/d;                                                                                  % Goal Attraction function
    options.func_terrain {mustBeFunction} = @(rho, d, d_min, vers)  rho*(1/d-1/d_min)/d*vers;                                                               % Terrain repulsion function
    options.func_obstacles {mustBeFunction} = @(rho, d, d_min, vers) rho*exp(-(0.33/d_min)*(d)^2)/(d)*(vers);                                               % Obstacle repulsion function      
    options.r_inf_terrain (1,1) double {mustBePositive} = 15;                                                                                               % Influence radius of the terrain
    options.r_inf_obstacles (1,1) double {mustBePositive} = 20;                                                                                             % Influence radius of the obstacles
    options.epsilon (1,1) double = 0.5;                                                                                                                     % Stopping criterion (e.g., half the precision)
    options.max_iter (1,1) double {mustBePositive, mustBeInteger} = 1000;                                                                                   % Stopping criterion to avoid infinite execution                                                   
    options.in_formation_threshold (1,1) double = 5;                                                                                                        % Trashold under which the drones meets into a single point
    options.precision (1,1) double = 1;              
end

rho = options.rho;
min_multiplier = options.min_multiplier;
max_multiplier = options.max_multiplier;
rho_drones = options.rho_drones;
rho_terrain = options.rho_terrain;
rho_obstacles = options.rho_obstacles;
func_attr = options.func_attr;
func_terrain = options.func_terrain;
func_obstacles = options.func_obstacles;
r_inf_terrain = options.r_inf_terrain;
r_inf_obstacles = options.r_inf_obstacles;
epsilon = options.epsilon;
max_iter = options.max_iter;
in_formation_threshold = options.in_formation_threshold; 
precision = options.precision;

% Initialization
path = zeros(max_iter,size(start,1), 3);
path(1,:,:) = start;
p_new = zeros(size(start,1),3);
p = start;
d_pg = vecnorm(p - goal, 2, 2);
rel_dist = Inf(size(start, 1), size(start, 1));
rel_dist(logical(eye(size(rel_dist)))) = 0;
connected = eye(size(start,1));
meeting_points = cell(1, 0); 
F_att_goal = zeros(size(start,1), 3);
nearest = 0;
tot_distance_ = zeros(size(start,1),1);
[max_x, max_y] = size(map);
iter = 1;
complete = false;

while all(d_pg > epsilon) && iter <= max_iter && ~complete
    drones = 1;
    while drones <= size(start,1)
        if path(iter,drones, :) == 0

            % Attraction force drones
            F_att_drones = [0, 0, 0];
            
            nearest_dist = Inf;

            for j = 1:size(start,1) 
                if j ~= drones && ~connected(drones,j) && rel_dist(drones,j) < nearest_dist
                    nearest = j;
                    nearest_dist = rel_dist(drones,j);
                end
            end    

            if nearest_dist < Inf  

                if iter > 2
                    for h = 1: size(start,1)
                        path_h = reshape(path(:, h, :), [], size(path, 3));
                        if h == drones
                            tot_distance_(h) = tot_distance_(h) + norm(path_h(iter-2,:) - path_h(iter-1,:));
                        end
                    end
                    mid_distance = sum(tot_distance_)/ size(start, 1);
                    drone_distance = tot_distance_(drones);
                else
                    mid_distance = 1;
                    drone_distance = 1;
                end

                F_att_drones = func_attr(rho_drones, p(drones,:), p(nearest,:),nearest_dist) * (min_multiplier + (max_multiplier - min_multiplier) * (exp(mid_distance / drone_distance) - 1) / (exp(1) - 1));

            end
            
            % Attraction force goal
%             if iter > 2
%                 farest_dist_goal = 0;
%                 for j = 1:size(start,1) 
%                     if d_pg(j) > farest_dist_goal
%                         farest = j;
%                         farest_dist_goal = d_pg(j);
%                     end
%                 end     
%                 if drones == farest
%                     F_att_goal(drones,:) = func_attr(rho, p(drones,:), goal, farest_dist_goal);
%                 else
%                     F_att_goal(drones,:) = calcolaForzaB(p(farest,:), p(drones,:), F_att_goal(farest,:));
%                 end
%             else
%                 F_att_goal(drones,:) = func_attr(rho, p(drones,:), goal, d_pg(drones));
%             end

            % Terrain repulsion force
            F_rep_terr = terrain_repulsion(p(drones,:), map, r_inf_terrain, rho_terrain, func_terrain);

            % Obstacle repulsion force
            F_obs = [0, 0, 0];
            for i = 1:length(obstacles)
                F_obs = F_obs + obstacle_repulsion(p(drones,:), obstacles(i), r_inf_obstacles, rho_obstacles, func_obstacles);
            end

            F_rep = F_rep_terr + F_obs;
            F = F_att_goal(drones,:) + F_att_drones + F_rep;

            % Update point
            p_new(drones,:) = p(drones,:) + F;
            p_new(drones,1) = max(1, min(p_new(drones,1), max_x));
            p_new(drones,2) = max(1, min(p_new(drones,2), max_y));
            p_new(drones,3) = max(p_new(drones,3), map(floor(p_new(drones,1)), floor(p_new(drones,2))));
            d_pg(drones) = norm(p_new(drones,:) - goal);

            % Update relative distances and check for meeting points
            meet = false;

            for j = 1:size(start,1)
                if j ~= drones && ~connected(drones,j)

                    rel_dist(j,drones) = norm(p_new(drones,:) - p(j,:));
                    rel_dist(drones,j) = rel_dist(j,drones);

                    if rel_dist(j,drones) < in_formation_threshold

                        p_new(drones,:) = (p_new(drones,:) + p(j,:))/2;
                        connected(drones,:) = connected(drones,:) + connected(j,:);

                        for h = 1:size(start,1)
                            if h ~= drones && connected(drones,h)
                                connected(h,:) = connected(drones,:);
                                rel_dist(h,drones) = 0;
                                rel_dist(drones,h) = 0;
                            end
                        end

                        meet = true;
                    end
                end
            end

            if meet == true
                
                meeting_point.coordinates = p_new(drones,:);
                meeting_point.connected = connected(drones,:);
                meeting_point.distances = Inf * ones(size(start,1),1);

                for h = 1:size(start,1)
                    if h == drones || connected(drones,h)             
                        p(h,:) = meeting_point.coordinates;   
                        path(iter,h, :) = meeting_point.coordinates;
                        
                        path_h = reshape(path(:, h, :), [], size(path, 3));
                        meeting_point.distances(h) = tot_distance_(h) + norm(path_h(iter-1,:) - path_h(iter,:));
                    end
                end
                % Add meeting point
                meeting_points{end+1} = meeting_point;

                %If all the drones are connected end the algorithm
                if all(connected(drones,:))
                    complete = true;
                end
            else
                
                path(iter,drones, :) = p_new(drones,:);
                p(drones,:) = p_new(drones,:);
                
            end
            
        end
        drones = drones + 1;
     end
     iter = iter + 1;
%     rho = rho - 0.05;
%     if rho < 0.01
%         rho = 0.01;
%     end
end

path(iter:end,:,:) = [];
end


function F = terrain_repulsion(p, map, d_min, rho, func)
    % TODO: Check if it is best to keep only the force with the highest
    % norm or if it is better to sum the forces for any point that
    % generates one
    x_p = floor(p(1));
    y_p = floor(p(2));
    
    h = map(x_p, y_p);
    
    % Repulsion on the vertical axis
    dh = p(3) - h;
    d = dh^2;
    
    if d <= d_min
        F = func(rho, d, d_min, [0, 0, dh]);
    else
        F = [0, 0, 0];
    end
    
    % Check close coordinates to verify if there are any closer points
    x_min = max(1, floor(p(1) - sqrt(d_min)));
    x_max = min(size(map,1), ceil(p(1) + sqrt(d_min)));
    y_min = max(1, floor(p(2) - sqrt(d_min)));
    y_max = min(size(map,2), ceil(p(2) + sqrt(d_min)));
    
    for x = x_min:x_max
        for y = y_min:y_max
            ref = [x, y, min(p(3), map(x,y))];
            vers = p - ref;
            d = norm(vers);
            
            if d <= d_min
                % Non-vertical terrain repulsion
                F = F + func(rho/2, d, d_min, vers);
           end
        end
    end
end

function F = obstacle_repulsion(p, obstacle, d_min, rho, func)
    pr = obstacle.getClosestPoint(p);
    
    if obstacle.includesPoint3(p)
        % If p is inside the obstacle, no repulsion is applied
        F = [0, 0, 0];
    else
        vers = p - pr;
        d0 = norm(vers);
        
        if d0 <= d_min
            F = func(rho, d0, d_min, vers);
            
            % Examples of possible obstacle repulsion functions:
            % @(rho, d, d_min, vers) rho*exp(-(0.33/d_min)*(d)^2)/(d)*(vers);
            % @(rho, d, d_min, vers) rho-d*rho/d_min*vers/d;
        else
            F = [0, 0, 0];
        end
    end
end

function Fb = calcolaForzaB(A, B, Fa)
    % Calcola il punto medio tra A e B
    M = (A + B) / 2;

    % Trova il vettore direttore AB
    AB = B - A;

    % Trova un vettore perpendicolare a AB (prendendo il prodotto vettoriale con [0 0 1])
    perp_vector = cross(AB, [0 0 1]);

    % Assicurati che il vettore perpendicolare non sia nullo
    if norm(perp_vector) == 0
        perp_vector = [0 1 0];  % Usiamo un vettore perpendicolare predefinito se il vettore AB è parallelo all'asse z
    end

    % Normalizza il vettore della forza Fa per ottenere un vettore direzionale
    Fa_dir = Fa / norm(Fa);

    % Trova le coordinate del punto di intersezione tra la retta e la direzione della forza
    t = dot(M - A, Fa_dir) / dot(perp_vector, Fa_dir);

    % Trova le coordinate del punto di intersezione
    intersection_point = M + t * perp_vector;

    % Trova il vettore che unisce B con il punto di intersezione
    vector_B_intersection = intersection_point - B;

    % Normalizza il vettore che unisce B con il punto di intersezione
    vector_B_intersection_dir = vector_B_intersection / norm(vector_B_intersection);

    % Intensità della forza Fa
    intensita_Fa = norm(Fa);

    % Genera il vettore forza partente dal punto B con la stessa intensità di Fa
    Fb = intensita_Fa * vector_B_intersection_dir;
end