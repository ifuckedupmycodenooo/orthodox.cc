
 if not getgenv().Orthodox then return end
 
 if not game:IsLoaded() then game.Loaded:Wait() end
 
 loadstring(game:HttpGet("https://raw.githubusercontent.com/ifuckedupmycodenooo/myscriptargsandshit/refs/heads/main/bypasser",true))()
 local menu = loadstring(game:HttpGet("https://raw.githubusercontent.com/ifuckedupmycodenooo/orthodox/refs/heads/main/orthdox",true))()
 
 local players = game:GetService("Players")
 local runservice = game:GetService("RunService")
 local userinput = game:GetService("UserInputService")
 local virtualinput = game:GetService("VirtualInputManager")
 
 
 local localplayer = players.LocalPlayer
 local camera = workspace.CurrentCamera
 local mouse = localplayer:GetMouse()
 
 local targeting = false
 local target = nil
 local silent_target = nil
 local macroing = false
 local resolving = false
 
 local last_update = tick()
 local old_position = Vector3.new()
 local remote, args_table = nil, nil
 
 
 utility = {}
 
 utility.mouse = function()
    return userinput:GetMouseLocation()  
 end
 
 utility.input = function(keycode)
    if typeof(keycode) == "string" then
        if keycode:match("MouseButton%d+") then
            return userinput:IsMouseButtonPressed(Enum.UserInputType[keycode])
        end
        return userinput:IsKeyDown(Enum.KeyCode[keycode])
    end
    return false
 end
 
 utility.notify = function(message)
    if Orthodox.StartUp.Notifications.Enabled then
        menu.Notify('Orthodox.<font color="#'..tostring(Orthodox.StartUp.Notifications.Color:ToHex())..'">.lua</font> | '..message, Orthodox.StartUp.Notifications.Duration)
    end
 end
 
 utility.create_circle = function(radius, color, thickness)
    local lines = {}
    local segments = math.floor(radius / 15)
    for i = 1, segments do
        local line = Drawing.new("Line")
        line.Visible = true
        line.Color = color
        line.Thickness = thickness
        line.Transparency = 1
        table.insert(lines, line)
    end
    return lines
 end
 
 utility.update_circle = function(lines, radius, position, color, thickness)
    local segments = #lines
    for i = 1, segments do
        local line = lines[i]
        line.Color = color
        line.Thickness = thickness
        
        local start_angle = ((i - 1) * 2 * math.pi / segments)
        local end_angle = start_angle + (2 * math.pi / segments)
        
        line.From = Vector2.new(
            position.X + math.cos(start_angle) * radius,
            position.Y + math.sin(start_angle) * radius
        )
        
        line.To = Vector2.new(
            position.X + math.cos(end_angle) * radius,
            position.Y + math.sin(end_angle) * radius
        )
    end
 end
 
 utility.lighting_defaults = {
    ambient = game:GetService("Lighting").Ambient,
    outdoor = game:GetService("Lighting").OutdoorAmbient,
    fog_color = game:GetService("Lighting").FogColor,
    fog_start = game:GetService("Lighting").FogStart,
    fog_end = game:GetService("Lighting").FogEnd
 }
 
 utility.setup_lighting = function()
    local lighting = game:GetService("Lighting")
    if lighting then
        utility.lighting_defaults = {
            ambient = lighting.Ambient,
            outdoor = lighting.OutdoorAmbient,
            fog_color = lighting.FogColor,
            fog_start = lighting.FogStart,
            fog_end = lighting.FogEnd
        }
    end
 end
 
 utility.update_ambience = function()
    local lighting = game:GetService("Lighting")
    if Orthodox.Visuals.Ambience.Enabled then
        lighting.Ambient = Orthodox.Visuals.Ambience.Color
        lighting.OutdoorAmbient = Orthodox.Visuals.Ambience.Color
    else 
        lighting.Ambient = utility.lighting_defaults.ambient
        lighting.OutdoorAmbient = utility.lighting_defaults.outdoor
    end
 end
 
 utility.update_fog = function()
    local lighting = game:GetService("Lighting")
    if Orthodox.Visuals.Fog.Enabled then
        lighting.FogColor = Orthodox.Visuals.Fog.Color
        lighting.FogStart = Orthodox.Visuals.Fog.Start
        lighting.FogEnd = Orthodox.Visuals.Fog.End
    else
        lighting.FogColor = utility.lighting_defaults.fog_color
        lighting.FogStart = utility.lighting_defaults.fog_start
        lighting.FogEnd = utility.lighting_defaults.fog_end
    end
 end
 
 utility.setup_lighting()
 
 utility.play_intro = function()
    local introgui = Instance.new("ScreenGui")
    local introframe = Instance.new("Frame")
    local introimage = Instance.new("ImageLabel")
    local introsound = Instance.new("Sound")
    local motionblureffect = Instance.new("BlurEffect")
 
    introgui.Name = "IntroScreen"
    introgui.Parent = game.Players.LocalPlayer:WaitForChild("PlayerGui")
 
    introframe.Name = "IntroFrame"
    introframe.Parent = introgui
    introframe.Size = UDim2.new(1, 0, 1, 0)
    introframe.BackgroundTransparency = 1
 
    introimage.Name = "IntroImage"
    introimage.Parent = introframe
    introimage.Size = UDim2.new(0.4, 0, 0.4, 0)
    introimage.Position = UDim2.new(0.3, 0, 0.3, 0)
    introimage.Image = "rbxassetid://123098524227942"
    introimage.BackgroundTransparency = 1
    introimage.ImageTransparency = 1
 
    introsound.Name = "IntroSound"
    introsound.Parent = introframe
    introsound.SoundId = "rbxassetid://6580172940"
    introsound.Volume = 15
    introsound:Play()
 
    motionblureffect.Parent = game.Lighting
    motionblureffect.Size = 24
 
    local tweenservice = game:GetService("TweenService")
    local tweeninfo = TweenInfo.new(2, Enum.EasingStyle.Quad, Enum.EasingDirection.Out, 0, false, 1)
    
    local fadein = tweenservice:Create(introimage, tweeninfo, {ImageTransparency = 0.5})
    local fadeout = tweenservice:Create(introimage, tweeninfo, {ImageTransparency = 1})
    
    fadein:Play()
    fadein.Completed:Wait()
    wait(2)
    fadeout:Play()
    
    local blurtweeninfo = TweenInfo.new(3, Enum.EasingStyle.Quad, Enum.EasingDirection.Out)
    local blurtween = tweenservice:Create(motionblureffect, blurtweeninfo, {Size = 0})
    
    blurtween:Play()
    blurtween.Completed:Wait()
    motionblureffect:Destroy()
    fadeout.Completed:Wait()
    introgui:Destroy()
 end
 
 utility.get_closest = function()
    local closest, distance = nil, math.huge
    local mousepos = utility.mouse()
    
    for _, player in pairs(players:GetPlayers()) do
        if player ~= localplayer and player.Character and player.Character:FindFirstChild("HumanoidRootPart") then
            local pos = camera:WorldToViewportPoint(player.Character.HumanoidRootPart.Position)
            local mag = (Vector2.new(pos.X, pos.Y) - mousepos).Magnitude
            
            if mag < Orthodox.Camlock.Radius and mag < distance then
                closest = player
                distance = mag
            end
        end
    end
    return closest
 end
 
 utility.get_closest_2 = function()
    local closest, distance = nil, math.huge
    local mousepos = utility.mouse()
    
    for _, player in pairs(players:GetPlayers()) do
        if player ~= localplayer and player.Character and player.Character:FindFirstChild("HumanoidRootPart") then
            local pos = camera:WorldToViewportPoint(player.Character.HumanoidRootPart.Position)
            local mag = (Vector2.new(pos.X, pos.Y) - mousepos).Magnitude
            
            if mag < Orthodox.Silent.Radius and mag < distance then
                closest = player
                distance = mag
            end
        end
    end
    return closest
 end
 
 utility.get_closest_part = function(char)
    local closest, distance = nil, math.huge
    local mousepos = utility.mouse()
    
    for _, part in pairs(char:GetChildren()) do
        if part:IsA("BasePart") then
            local pos = camera:WorldToViewportPoint(part.Position)
            local mag = (Vector2.new(pos.X, pos.Y) - mousepos).Magnitude
            
            if mag < distance then
                closest = part
                distance = mag
            end
        end
    end
    return closest
 end
 
 utility.predict_target = function(position, target_type, player)
    if not player or not player.Character then return position end
    
    local targetpart = player.Character:FindFirstChild(Orthodox[target_type].HitPart)
    if Orthodox[target_type].ClosestPart then
        targetpart = utility.get_closest_part(player.Character)
    end
    
    if not targetpart then return position end
    if not targetpart.Velocity then return position end
    
    if resolving then
        if old_position == Vector3.new() then
            old_position = targetpart.Position
            return targetpart.Position + (targetpart.Velocity * Orthodox[target_type].Prediction)
        end
        
        local delta_time = tick() - last_update
        last_update = tick()
        local rv = (targetpart.Position - old_position) / delta_time
        old_position = targetpart.Position
        return targetpart.Position + (rv * Orthodox[target_type].Prediction)
    end
    
    return targetpart.Position + (targetpart.Velocity * Orthodox[target_type].Prediction)
 end
 
 utility.get_remote_args = function()
    local mt = getrawmetatable(game)
    local old_namecall = mt.__namecall
    local old_index = mt.__index
    setreadonly(mt, false)
 
    mt.__namecall = newcclosure(function(self, ...)
        local args = {...}
        local method = getnamecallmethod()
        
        if not checkcaller() and method == "FireServer" then
            remote = self
            args_table = args
        end
        
        return old_namecall(self, ...)
    end)
 
    mt.__index = newcclosure(function(self, k)
        if not checkcaller() and k == "Hit" then
            local player = Orthodox.Silent.Enabled and silent_target or target
            if player and player.Character then
                local targetpart
                if Orthodox.Silent.ClosestPart then
                    targetpart = utility.get_closest_part(player.Character)
                else
                    targetpart = player.Character:FindFirstChild(Orthodox.Silent.Enabled and Orthodox.Silent.HitPart or Orthodox.Camlock.HitPart)
                end
                
                if targetpart then
                    return CFrame.new(utility.predict_target(targetpart.Position, Orthodox.Silent.Enabled and "Silent" or "Camlock", player))
                end
            end
        end
        return old_index(self, k)
    end)
 
    setreadonly(mt, true)
 end
 
 utility.shoot = function(tool)
    if not tool then return end
    
    tool.Activated:Connect(function()
        local player = Orthodox.Silent.Enabled and silent_target or target
        if not player then return end
        
        local targetpart
        if Orthodox.Silent.ClosestPart then
            targetpart = utility.get_closest_part(player.Character)
        else
            targetpart = player.Character:FindFirstChild(Orthodox.Silent.Enabled and Orthodox.Silent.HitPart or Orthodox.Camlock.HitPart)
        end
        
        if not targetpart then return end
        
        local predicted = utility.predict_target(targetpart.Position, Orthodox.Silent.Enabled and "Silent" or "Camlock", player)
        
        if remote and args_table then
            for i,v in pairs(args_table) do
                if typeof(v) == "Vector3" then
                    args_table[i] = predicted
                elseif typeof(v) == "CFrame" then
                    args_table[i] = CFrame.new(predicted)
                elseif type(v) == "table" then
                    for j,k in pairs(v) do
                        if typeof(k) == "Vector3" then
                            v[j] = predicted
                        elseif typeof(k) == "CFrame" then
                            v[j] = CFrame.new(predicted)
                        end
                    end
                end
            end
            remote:FireServer(unpack(args_table))
        end
    end)
 end
 
 
 local camlock_circle = utility.create_circle(Orthodox.Camlock.Radius, Orthodox.FOV.Camlock.Color, Orthodox.FOV.Camlock.Thickness)
 local silent_circle = utility.create_circle(Orthodox.Silent.Radius, Orthodox.FOV.Silent.Color, Orthodox.FOV.Silent.Thickness)
 
 
 runservice.RenderStepped:Connect(function()
    local mouse_pos = utility.mouse()
    
    for _, line in pairs(camlock_circle) do
        line.Visible = Orthodox.FOV.Camlock.Visible
    end
 
    for _, line in pairs(silent_circle) do
        line.Visible = Orthodox.FOV.Silent.Visible
    end
    
    utility.update_circle(camlock_circle, Orthodox.Camlock.Radius, mouse_pos, Orthodox.FOV.Camlock.Color, Orthodox.FOV.Camlock.Thickness)
    utility.update_circle(silent_circle, Orthodox.Silent.Radius, mouse_pos, Orthodox.FOV.Silent.Color, Orthodox.FOV.Silent.Thickness)
 
    silent_target = utility.get_closest_2()
    
    if Orthodox.Camlock.Binding.Mode == "Hold" then
        targeting = utility.input(Orthodox.Camlock.Binding.Key)
        target = targeting and utility.get_closest() or nil
    end
    
    if targeting and target and target.Character then
        local hitpart = target.Character:FindFirstChild(Orthodox.Camlock.HitPart)
        if hitpart then
            if Orthodox.Checks.Death and target.Character.Humanoid.Health <= 2.5 then
                targeting = false
                target = nil
                return
            end
            
            local position = utility.predict_target(hitpart.Position, "Camlock", target)
            local targetcf = CFrame.new(camera.CFrame.Position, position)
            
            camera.CFrame = camera.CFrame:Lerp(targetcf, 
                Orthodox.Camlock.Smoothness/10, 
                Enum.EasingStyle[Orthodox.Camlock.Easing.Style], 
                Enum.EasingDirection[Orthodox.Camlock.Easing.Direction])
        end
    end
 
    
    if Orthodox.Movement.Macro.Enabled and macroing then
        if Orthodox.Movement.Macro.Mode == "Third" and localplayer.Character:FindFirstChild("HumanoidRootPart") and camera then
            local look = camera.CFrame.LookVector
            local macro = localplayer.Character.HumanoidRootPart
            if look and macro then
                macro.CFrame = CFrame.new(macro.Position, macro.Position + Vector3.new(look.X, 0, look.Z))
            end
        end
    end
 end)
 
 
 userinput.InputBegan:Connect(function(input, processed)
    if processed then return end
 
    if Orthodox.Camlock.Binding.Key:match("MouseButton%d+") then
        if input.UserInputType == Enum.UserInputType[Orthodox.Camlock.Binding.Key] then
            if Orthodox.Camlock.Binding.Mode == "Toggle" then
                targeting = not targeting
                target = targeting and utility.get_closest() or nil
                if target then
                    utility.notify(targeting and "Targeting "..target.DisplayName or "Stopped Targeting")
                end
            end
        end
    else
        if input.KeyCode == Enum.KeyCode[Orthodox.Camlock.Binding.Key] then
            if Orthodox.Camlock.Binding.Mode == "Toggle" then
                targeting = not targeting
                target = targeting and utility.get_closest() or nil
                if target then
                    utility.notify(targeting and "Targeting "..target.DisplayName or "Stopped Targeting")
                end
            end
        end
    end
 
    if input.KeyCode == Enum.KeyCode[Orthodox.Checks.Resolver.Key] and Orthodox.Checks.Resolver.Enabled then
        resolving = not resolving
        if resolving then
            last_update = tick()
            old_position = Vector3.new()
        end
        utility.notify(resolving and "Resolving..." or "Stopped Resolving")
    end
 
    if input.KeyCode == Enum.KeyCode[Orthodox.Movement.Macro.Key] and Orthodox.Movement.Macro.Enabled then
        macroing = not macroing
        utility.notify(macroing and "Macroing" or "Stopped Macroing")
        
        
     if macroing and Orthodox.Movement.Macro.Mode == "First" then
         task.spawn(function()
         while macroing do
             local waittime = Orthodox.Movement.Macro.Delay
             task.wait(waittime)
             virtualinput:SendMouseWheelEvent("0", "0", true, game)
             task.wait(waittime)
             virtualinput:SendMouseWheelEvent("0", "0", false, game)
             task.wait(waittime)
             virtualinput:SendMouseWheelEvent("0", "0", true, game)
             task.wait(waittime)
             virtualinput:SendMouseWheelEvent("0", "0", false, game)
             
             if macroing and localplayer.Character:FindFirstChild("HumanoidRootPart") and camera then
                 local look = camera.CFrame.LookVector
                 local macro = localplayer.Character.HumanoidRootPart
                 if look and macro then
                     macro.CFrame = CFrame.new(macro.Position, macro.Position + Vector3.new(look.X, 0, look.Z))
                 end
             end
          end
       end)
     end
  end
 
    if input.KeyCode == Enum.KeyCode[Orthodox.Visuals.Ambience.Toggle] then
        Orthodox.Visuals.Ambience.Enabled = not Orthodox.Visuals.Ambience.Enabled
        utility.notify(Orthodox.Visuals.Ambience.Enabled and "Enabled Ambience" or "Disabled Ambience")
        utility.update_ambience()
    end
 
    if input.KeyCode == Enum.KeyCode[Orthodox.Visuals.Ambience.Reset] then
        Orthodox.Visuals.Ambience.Enabled = false
        Orthodox.Visuals.Fog.Enabled = false
        utility.notify("Reset Visuals")
        utility.update_ambience()
        utility.update_fog()
    end
 end)
 
 
 if Orthodox.StartUp.Intro then 
    utility.play_intro() 
 end
 
 if localplayer.Character then
    for _, tool in pairs(localplayer.Character:GetChildren()) do
        if tool:IsA("Tool") then
            utility.shoot(tool)
        end
    end
 end
 
 localplayer.Character.ChildAdded:Connect(function(tool)
    if tool:IsA("Tool") then
        utility.shoot(tool)
    end
 end)
 
 localplayer.CharacterAdded:Connect(function(character)
    character.ChildAdded:Connect(function(tool)
        if tool:IsA("Tool") then
            utility.shoot(tool)
        end
    end)
 end)
 
 utility.get_remote_args()
