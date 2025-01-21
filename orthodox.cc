 if not getgenv().Orthodox then return end
 
 if not game:IsLoaded() then game.Loaded:Wait() end
 
 loadstring(game:HttpGet("https://raw.githubusercontent.com/vininx/bypasserlarlaprlalrp-/refs/heads/main/larplarpbypasser"))()
 local menu = loadstring(game:HttpGet("https://raw.githubusercontent.com/ifuckedupmycodenooo/orthodox/refs/heads/main/orthdox",true))()
 
 -- // services
 local players = game:GetService("Players")
 local runservice = game:GetService("RunService")
 local userinput = game:GetService("UserInputService")
 local virtualinput = game:GetService("VirtualInputManager")
 
 -- // instances
 local localplayer = players.LocalPlayer
 local camera = workspace.CurrentCamera
 local mouse = localplayer:GetMouse()
 
 -- // states
 local targeting = false
 local target = nil
 local macroing = false
 local resolving = false
 
 -- // variables
 local last_update = tick()
 local old_position = Vector3.new()
 local remote, args_table = nil, nil
 
 utility = {}
 
 utility.notify = function(message)
    if Orthodox.StartUp.Notifications.Enabled then
        menu.Notify('Orthodox.<font color="#'..tostring(Orthodox.StartUp.Notifications.Color:ToHex())..'">.lua</font> | '..message, Orthodox.StartUp.Notifications.Duration)
    end
 end
 
 utility.play_intro = function()
    local introGui = Instance.new("ScreenGui")
    local introFrame = Instance.new("Frame")
    local introImage = Instance.new("ImageLabel")
    local introSound = Instance.new("Sound")
    local motionBlurEffect = Instance.new("BlurEffect")
 
    introGui.Name = "IntroScreen"
    introGui.Parent = game.Players.LocalPlayer:WaitForChild("PlayerGui")
 
    introFrame.Name = "IntroFrame"
    introFrame.Parent = introGui
    introFrame.Size = UDim2.new(1, 0, 1, 0)
    introFrame.BackgroundTransparency = 1
 
    introImage.Name = "IntroImage"
    introImage.Parent = introFrame
    introImage.Size = UDim2.new(0.4, 0, 0.4, 0)
    introImage.Position = UDim2.new(0.3, 0, 0.3, 0)
    introImage.Image = "rbxassetid://132337978477296"
    introImage.BackgroundTransparency = 1
    introImage.ImageTransparency = 1
 
    introSound.Name = "IntroSound"
    introSound.Parent = introFrame
    introSound.SoundId = "rbxassetid://6580172940"
    introSound.Volume = 15
    introSound:Play()
 
    motionBlurEffect.Parent = game.Lighting
    motionBlurEffect.Size = 24
 
    local tweenService = game:GetService("TweenService")
    local tweenInfo = TweenInfo.new(2, Enum.EasingStyle.Quad, Enum.EasingDirection.Out, 0, false, 1)
    
    local fadeInTween = tweenService:Create(introImage, tweenInfo, {ImageTransparency = 0.5})
    local fadeOutTween = tweenService:Create(introImage, tweenInfo, {ImageTransparency = 1})
    
    fadeInTween:Play()
    fadeInTween.Completed:Wait()
    wait(2)
    fadeOutTween:Play()
    
    local blurTweenInfo = TweenInfo.new(3, Enum.EasingStyle.Quad, Enum.EasingDirection.Out)
    local blurTween = tweenService:Create(motionBlurEffect, blurTweenInfo, {Size = 0})
    
    blurTween:Play()
    blurTween.Completed:Wait()
    motionBlurEffect:Destroy()
    fadeOutTween.Completed:Wait()
    introGui:Destroy()
 end
 
 utility.get_closest = function()
    local closest, distance = nil, math.huge
    local mousepos = userinput:GetMouseLocation()
    
    for _, player in pairs(players:GetPlayers()) do
        if player ~= localplayer and player.Character and player.Character:FindFirstChild("Humanoid") 
        and player.Character.Humanoid.Health > 0 then
            local pos = camera:WorldToViewportPoint(player.Character.PrimaryPart.Position)
            local mag = (Vector2.new(pos.X, pos.Y) - mousepos).Magnitude
            
            if Orthodox.FOV.Enabled then
                if mag < Orthodox.FOV.Radius and mag < distance then
                    closest = player
                    distance = mag
                end
            elseif mag < distance then
                closest = player
                distance = mag
            end
        end
    end
    return closest
 end
 
 utility.predict_target = function(position)
    if not target or not target.Character then return position end
    local targetpart = target.Character:FindFirstChild(Orthodox.Camlock.HitPart)
    if not targetpart then return position end
    
    if resolving then
        local delta_time = tick() - last_update
        last_update = tick()
        local rv = (targetpart.Position - old_position) / delta_time
        old_position = targetpart.Position
        return targetpart.Position + (targetpart.Velocity * Orthodox.Camlock.Prediction)
    end
    
    return targetpart.Position + (targetpart.Velocity * Orthodox.Camlock.Prediction)
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
    setreadonly(mt, true)
 end
 
 utility.shoot = function(tool)
    if not tool then return end
    
    tool.Activated:Connect(function()
        if not target then return end
        local targetpart = target.Character and target.Character:FindFirstChild(Orthodox.Camlock.HitPart)
        if not targetpart then return end
 
        local predicted = utility.predict_target(targetpart.Position)
        
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
 
 -- // draw fov circle
 local circle = Drawing.new("Circle")
 circle.Visible = Orthodox.FOV.Enabled
 circle.Color = Orthodox.FOV.Color
 circle.Thickness = 1
 circle.Transparency = Orthodox.FOV.Transparency
 circle.NumSides = 60
 circle.Radius = Orthodox.FOV.Radius
 circle.Filled = Orthodox.FOV.Filled
 
 runservice.RenderStepped:Connect(function()
    circle.Position = userinput:GetMouseLocation()
    
    if targeting and target and target.Character then
        local hitpart = target.Character:FindFirstChild(Orthodox.Camlock.HitPart)
        if hitpart then
            if Orthodox.Checks.Death and target.Character.Humanoid.Health <= 2.5 then
                targeting = false
                target = nil
                return
            end
            
            local position = utility.predict_target(hitpart.Position)
            local targetcf = CFrame.new(camera.CFrame.Position, position)
            
            camera.CFrame = camera.CFrame:Lerp(targetcf, 
                Orthodox.Camlock.Smoothness/10, 
                Enum.EasingStyle[Orthodox.Camlock.Easing.Style], 
                Enum.EasingDirection[Orthodox.Camlock.Easing.Direction])
        end
    end
 end)
 
 userinput.InputBegan:Connect(function(input, processed)
    if processed then return end
    
    if input.KeyCode == Enum.KeyCode[Orthodox.Camlock.Key] then
        targeting = not targeting
        target = targeting and utility.get_closest() or nil
        if target then
            utility.notify(targeting and "Targeting "..target.DisplayName or "Stopped Targeting")
        end
    elseif input.KeyCode == Enum.KeyCode[Orthodox.Checks.Resolver.Key] and Orthodox.Checks.Resolver.Enabled then
        resolving = not resolving
        utility.notify(resolving and "Resolving..." or "Stopped Resolving")
    elseif input.KeyCode == Enum.KeyCode[Orthodox.Movement.Macro.Key] and Orthodox.Movement.Macro.Enabled then
        macroing = not macroing
        utility.notify(macroing and "Macroing" or "Stopped Macroing")
        if macroing then
            repeat
                local delay = Orthodox.Movement.Macro.Delay / 100
                task.wait(delay)
                if Orthodox.Movement.Macro.Mode == "Third" then
                    virtualinput:SendKeyEvent(true, "I", false, game)
                    task.wait(delay)
                    virtualinput:SendKeyEvent(true, "O", false, game)
                elseif Orthodox.Movement.Macro.Mode == "First" then
                    virtualinput:SendMouseWheelEvent("0", "0", true, game)
                    task.wait(delay)
                    virtualinput:SendMouseWheelEvent("0", "0", false, game)
                end
            until not macroing
        end
    end
 end)
 
 if Orthodox.StartUp.Intro then utility.play_intro() end
 
 utility.get_remote_args()
 
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
