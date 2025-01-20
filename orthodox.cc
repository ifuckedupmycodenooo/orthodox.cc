local g = getinfo or debug.getinfo
local r = getrenv or function() return _G end
local c = getgc or function() return {} end
local h = {}
local x, y
local d = false

setthreadidentity(2)

for _, v in c(true) do
   if typeof(v) == "table" then
       local a = rawget(v, "Detected")
       local b = rawget(v, "Kill")
       
       if typeof(a) == "function" and not x then
           x = a
           local o = hookfunction(a, function() return true end)
           table.insert(h, o)
       end
       
       if rawget(v, "Variables") and rawget(v, "Process") and typeof(b) == "function" and not y then
           y = b
           local o = hookfunction(b, function() return end)
           table.insert(h, o)
       end
   end
end

hookfunction(r().debug.info, newcclosure(function(...)
   local a = ...
   if x and a == x then
       return coroutine.yield(coroutine.running())
   end
   return debug.info(...)
end))

setthreadidentity(7)

 local players = game:GetService("Players")
 local runservice = game:GetService("RunService") 
 local userinputservice = game:GetService("UserInputService")
 local camera = workspace.CurrentCamera
 local client = players.LocalPlayer
 
 local utility = {}
 local targeting = false
 local target = nil
 local old_position = Vector3.new()
 
 local last_update = 0
 local delta_time = 0
 
 utility.predict_target = function(current_pos)
    if not target or not target.Character then return current_pos end
    local targetPart = target.Character:FindFirstChild(getgenv().Orthodox.Aimbot.Target_Part)
    if not targetPart then return current_pos end
 
    delta_time = tick() - last_update
    last_update = tick()
 
    if utility.check_anti(target.Character) then
        local raw_velocity = (targetPart.Position - old_position) / delta_time
        local velocity = raw_velocity:Lerp(raw_velocity, 0.5)
        old_position = targetPart.Position
        return targetPart.Position + (velocity * getgenv().Orthodox.Aimbot.Prediction)
    end
    
    return targetPart.Position + (targetPart.Velocity * getgenv().Orthodox.Aimbot.Prediction)
 end

 utility.check_anti = function(char)
    if not getgenv().Orthodox.Global.Resolver then return false end
    local targetPart = char:FindFirstChild(getgenv().Orthodox.Aimbot.Target_Part)
    if not targetPart then return false end
    
    local velocity = targetPart.Velocity
    if velocity.Y < -70 or velocity.Y > 60 then return true end
    if velocity.X > 450 or velocity.X < -35 then return true end
    if velocity.Z > 35 or velocity.Z < -35 then return true end
    return false
 end
 
 utility.check_target = function(player)
    if not player or not player.Character or player == client then return false end
    
    if getgenv().Orthodox.Checks.Universal.Death then
        local humanoid = player.Character:FindFirstChild("Humanoid")
        if not humanoid or humanoid.Health <= 0 then return false end
    end
    
    if getgenv().Orthodox.Checks.Da_Hood.KO then
        local humanoid = player.Character:FindFirstChild("Humanoid")
        if humanoid and humanoid.Health <= 2 then return false end
    end
    
    if getgenv().Orthodox.Checks.Da_Hood.Grabbed then
        if player.Character:FindFirstChild("GRABBING_CONSTRAINT") then return false end
    end
    
    return true
end
 
 utility.get_target = function()
    local max_distance = getgenv().Orthodox.Visuals.Fov.Size
    local selected = nil
 
    for _, player in pairs(players:GetPlayers()) do
        if player ~= client and utility.check_target(player) then
            local root = player.Character[getgenv().Orthodox.Aimbot.Target_Part]
            local screen_position, on_screen = camera:WorldToScreenPoint(root.Position)
            local distance = (Vector2.new(screen_position.X, screen_position.Y) - userinputservice:GetMouseLocation()).Magnitude
            
            if distance < max_distance and on_screen then
                selected = player
                max_distance = distance
                old_position = root.Position
                last_update = tick()
            end
        end
    end
    
    target = selected
 end
 
 utility.setup_fov = function()
    local circle = Drawing.new("Circle")
    circle.Visible = getgenv().Orthodox.Visuals.Fov.Visible
    circle.Radius = getgenv().Orthodox.Visuals.Fov.Size
    circle.Color = getgenv().Orthodox.Visuals.Fov.Color
    circle.Transparency = getgenv().Orthodox.Visuals.Fov.Transparency
    circle.Filled = getgenv().Orthodox.Visuals.Fov.Filled
    circle.Thickness = getgenv().Orthodox.Visuals.Fov.Thickness
    circle.NumSides = getgenv().Orthodox.Visuals.Fov.Sides
    return circle
 end
 
 local fov_circle = utility.setup_fov()
 
 userinputservice.InputBegan:Connect(function(input)
    if input.KeyCode == getgenv().Orthodox.Aimbot.Binding.Keybind then
        if getgenv().Orthodox.Aimbot.Binding.Mode == "Toggle" then
            targeting = not targeting
            if not targeting then
                target = nil
                old_position = Vector3.new()
            else
                utility.get_target()
            end
        else
            targeting = true
            utility.get_target()
        end
    end
 end)
 
 userinputservice.InputEnded:Connect(function(input)
    if input.KeyCode == getgenv().Orthodox.Aimbot.Binding.Keybind and getgenv().Orthodox.Aimbot.Binding.Mode == "Hold" then
        targeting = false
        target = nil
        old_position = Vector3.new()
    end
 end)
 
 local remote, args_table = nil, nil

 utility.get_remote_args = function()
    local mt = getrawmetatable(game)
    local old = mt.__namecall
    setreadonly(mt, false)
 
    mt.__namecall = newcclosure(function(self, ...)
        local args = {...}
        local method = getnamecallmethod()
        
        if not checkcaller() and method == "FireServer" then
            remote = self
            args_table = args
        end
        
        return old(self, ...)
    end)
 
    setreadonly(mt, true)
 end
 
 utility.shoot = function(tool)
    if not tool then return end
    tool.Activated:Connect(function()
        if not target or not remote or not args_table then return end
        local targetPart = target.Character[getgenv().Orthodox.Aimbot.Target_Part]
        if not targetPart then return end
            
        local predicted = utility.predict_target(targetPart.Position)
        
        for i,v in pairs(args_table) do
            if typeof(v) == "Vector3" or typeof(v) == "CFrame" then
                args_table[i] = predicted
            elseif type(v) == "table" then
                for j,k in pairs(v) do
                    if typeof(k) == "Vector3" or typeof(k) == "CFrame" then
                        v[j] = predicted
                    end
                end
            end
        end
        
        remote:FireServer(unpack(args_table))
    end)
 end
 
 if client.Character then
    utility.get_remote_args()
    for _, tool in pairs(client.Character:GetChildren()) do
        if tool:IsA("Tool") then
            utility.shoot(tool)
        end
    end
 end
 
 client.Character.ChildAdded:Connect(function(tool)
    if tool:IsA("Tool") then
        utility.shoot(tool)
    end
 end)
 
 client.CharacterAdded:Connect(function(character)
    utility.get_remote_args()
    character.ChildAdded:Connect(function(tool)
        if tool:IsA("Tool") then
            utility.shoot(tool)
        end
    end)
 end)
 
 runservice.RenderStepped:Connect(function()
    if fov_circle then
        fov_circle.Position = userinputservice:GetMouseLocation()
        fov_circle.Visible = getgenv().Orthodox.Visuals.Fov.Visible
    end
 
    if getgenv().Orthodox.Aimbot.Enabled and targeting and target and utility.check_target(target) then
        local root = target.Character[getgenv().Orthodox.Aimbot.Target_Part]
        local predicted = utility.predict_target(root.Position)
        
        camera.CFrame = camera.CFrame:Lerp(CFrame.new(camera.CFrame.Position, predicted), getgenv().Orthodox.Aimbot.Smoothness)
        old_position = root.Position
    end
 end)
