// Author: Alex Hartford
// Program: Engine
// File: Editor
// Date: March 2023

#ifndef EDITOR_H
#define EDITOR_H

void RenderContextEditor(RenderContext *context,
                         const Shaders &shaders)
{
    ImGui::Begin("Render Context");
    {
        ImGui::SliderFloat("camera speed", &slow_movement_speed, 0.1f, 10.0f);
        ImGui::SliderFloat3("light dir", (float *)&(context->light_direction), -1.0f, 1.0f);
        ImGui::ColorEdit4("color", (float *)&(context->default_color));
        ImGui::Checkbox("use color", &(context->use_default_color));
        ImGui::ColorEdit4("background color", (float *)&(context->background_color));
        ImGui::SliderFloat("brightness", &(context->brightness), 0.0f, 3.0f);
        ImGui::SliderFloat("opacity", &(context->opacity), 0.0f, 1.0f);
        ImGui::SliderFloat("cel high", &(context->cel_threshold_high), -1.0f, 1.0f);
        ImGui::SliderFloat("cel mid", &(context->cel_threshold_mid), -1.0f, 1.0f);
        ImGui::SliderFloat("cel low", &(context->cel_threshold_low), -1.0f, 1.0f);
        ImGui::Checkbox("draw lines", &(context->draw_lines));
        ImGui::Checkbox("skybox", &(context->skybox));
        ImGui::Checkbox("processing", &(context->processing));
        ImGui::Checkbox("water", &(context->water));
        if(ImGui::Button("No Focus")) {
            camera.focus_index = -1;
        }
        //ImGui::Checkbox("shadow", &(context->shadow));

        /*
        if(ImGui::TreeNode("Shaders"))
        {
            for (int n = 0; n < shaders.list.size(); n++)
            {
                char buf[32];
                sprintf(buf, "%s", shaders.list[n].name.c_str());
                if(ImGui::Selectable(buf, context->shader_index == n))
                    context->shader_index = n;
            }
            ImGui::TreePop();
        }
        */
    }
    ImGui::End();
}

void LineEditor(Line *line, Line *smooth)
{
    ImGui::Begin("Line Editor");
    {
        static int line_selection = 0;

        if(ImGui::BeginListBox("points"))
        {
            for (int n = 0; n < line->points.size(); n++)
            {
                char buf[32];
                sprintf(buf, "%d", n);
                if(ImGui::Selectable(buf, line_selection == n))
                    line_selection = n;
            }
            ImGui::EndListBox();
        }
        if(ImGui::Button("Add"))
            line->points.push_back(line->points.back());
        if(ImGui::Button("Remove")) {
            line->points.erase (line->points.begin()+line_selection);
            line_selection = std::max(0, line_selection - 1);
        }
        if(ImGui::SliderFloat3("point", (float *)&(line->points[line_selection]), -100.0, 100.0))
        {
            line->BindBuffers();
        }
        if(ImGui::Button("Update Spline"))
        {
            vector<vec3> splinepoints;
            spline(splinepoints, line->points, SPLINE_LOD, 1.0);
            smooth->points = splinepoints;
            smooth->BindBuffers();
        }

    }
    ImGui::End();
}


void ObjectEditor(vector<Object> *level_objects,
                  EditorCommands *editor_commands)
{
    // Which Object are we editing?
    static Object *selected;
    if(GlobalEditorState.selected != -1)
        selected = &(level_objects->at(GlobalEditorState.selected));
    else
        selected = nullptr;

    if(!selected) return;

    // Local Functions
    auto process_move_command = [editor_commands](Object *selected, const mat4 &start, const mat4 &end) -> void
    {
        shared_ptr<Command> manip = make_shared<MoveObjectCommand>(selected, start, end);
        manip->Execute();
        editor_commands->AddToHistory(manip);
    };
    auto process_destroy_command = [editor_commands](vector<Object> *level_objects, int index) -> void
    {
        shared_ptr<Command> destroy = make_shared<DestroyObjectCommand>(level_objects, index);
        destroy->Execute();
        editor_commands->AddToHistory(destroy);
    };
    auto process_copy_command = [editor_commands](vector<Object> *level_objects, const Object &original) -> void
    {
        shared_ptr<Command> copy = make_shared<AddObjectCommand>(level_objects, original);
        copy->Execute();
        editor_commands->AddToHistory(copy);
    };

    // ============================ ImGui Start ================================
    ImGui::Begin("Object Editor");
    {
        ImGui::Text("%d | %s", GlobalEditorState.selected, selected->name.c_str());
        ImGui::SameLine();
        if(ImGui::Button("Destroy"))
            process_destroy_command(level_objects, GlobalEditorState.selected);
        ImGui::SameLine();
        if(ImGui::Button("Copy"))
            process_copy_command(level_objects, *selected);

        if(ImGui::Button("Focus"))
            camera.focus_index = GlobalEditorState.selected;

        static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
        static ImGuizmo::MODE      mode      = ImGuizmo::WORLD;

        if(ImGui::IsKeyPressed('1'))
            operation = ImGuizmo::TRANSLATE;
        if(ImGui::IsKeyPressed('2'))
            operation = ImGuizmo::ROTATE;
        if(ImGui::IsKeyPressed('3'))
            operation = ImGuizmo::SCALE;

        if(ImGui::RadioButton("Tr", operation == ImGuizmo::TRANSLATE))
            operation = ImGuizmo::TRANSLATE;

        ImGui::SameLine();
        if(ImGui::RadioButton("Rot", operation == ImGuizmo::ROTATE))
            operation = ImGuizmo::ROTATE;

        ImGui::SameLine();
        if(ImGui::RadioButton("Scl", operation == ImGuizmo::SCALE))
            operation = ImGuizmo::SCALE;

        ImGui::Text("Position: %.02f %.02f %.02f",
             selected->position.x, selected->position.y, selected->position.z);

        // ======================= Gizmos ======================================
        static bool movement_in_progress = false;
        static mat4 start;
        static mat4 end;

        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate((float *)&camera.view,
                             (float *)&camera.projection,
                             operation, mode, 
                             (float *)&(selected->matrix));

        // State machine. moving and not moving, and we use the IsUsing() to swap between them.
        if(movement_in_progress) { // State 1
            if(!ImGuizmo::IsUsing()) {
                movement_in_progress = false;
                end = selected->matrix;

                if(start != end)
                    process_move_command(selected, start, end);
            }
        }
        else // State 2
        {
            if(ImGuizmo::IsUsing()) {
                movement_in_progress = true;
                start = selected->matrix;
            }
        }

        if(ImGui::Button("origin")) {
            start = selected->matrix;
            end = GetMatrix(vec3(0.0f, 0.0f, 0.0f), 
                            selected->orientation, 
                            selected->scale_factor);
            process_move_command(selected, start, end);
        }
        ImGui::SameLine();
        if(ImGui::Button("plumb")) {
            start = selected->matrix;
            end = GetMatrix(selected->position,
                            quat(1.0f, 0.0f, 0.0f, 0.0f),
                            selected->scale_factor);
            process_move_command(selected, start, end);
        }
        ImGui::SameLine();
        if(ImGui::Button("1:1")) {
            start = selected->matrix;
            end = GetMatrix(selected->position,
                            selected->orientation,
                            1.0f);
            process_move_command(selected, start, end);
        }

        if (selected->animated) {
            if(ImGui::TreeNode("Animation")) {
                static int animation_selection = -1;

                if(selected->animator.current_animation) {
                    float progress = selected->animator.time / selected->animator.current_animation->duration;
                    char buf[32];
                    sprintf(buf, "%.01f/%.01f", selected->animator.time, selected->animator.current_animation->duration);
                    ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
                    ImGui::SameLine();
                    if(ImGui::Button("stop")) {
                        animation_selection = -1;
                        selected->animator.current_animation = NULL;
                    }

                    ImGui::SliderFloat("speed", &selected->animator.speed, 0.1f, 3.0f);
                }
                
                for (int n = 0; n < selected->animations.size(); n++)
                {
                    char buf[32];
                    sprintf(buf, "%d", n);
                    if(ImGui::Selectable(buf, animation_selection == n)) {
                        animation_selection = n;
                        selected->animator.PlayAnimation(selected->animations[n]);
                    }
                }

                ImGui::TreePop();
            }
        }

        ImGui::Checkbox("pathing", &selected->pathing);
        ImGui::SameLine();
        if(ImGui::Button("Reset")) {
            selected->pathing = false;
            selected->time = 0.0f;
            selected->segment = 0;
        }
        if(selected->pathing)
            LineEditor(&selected->line, &selected->path);
    }
    ImGui::End();
}

void Generation(const vector<Object> &palette,
                vector<Object> *level,
                const Terrain &terrain)
{
    ImGui::Begin("Generation");
    {
        int flowers[] = {30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48};
        int trees[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 19, 20, 21, 22, 23};
        int floor_cover[] = {25, 28, 29, 39};

        if (ImGui::Button("Flowers")) {
            for (int index : flowers) {
                Generate(5, palette[index], level, terrain);
            }
        }

        if (ImGui::Button("Trees")) {
            for (int index : trees) {
                Generate(5, palette[index], level, terrain);
            }
        }

        if (ImGui::Button("Ground Cover")) {
            for (int index : floor_cover) {
                Generate(5, palette[index], level, terrain);
            }
        }
    }
    ImGui::End();
}

void Palette(const vector<Object> &palette,
             vector<Object> *level_objects)
{
    static int palette_selection = -1;
    ImGui::Begin("Palette");
    {
        if(ImGui::Button("Add"))
        {
            if(palette_selection != -1)
                level_objects->push_back(Object(palette[palette_selection]));
                GlobalEditorState.selected = level_objects->size() - 1;
        }

        if (ImGui::BeginListBox("## 2", ImVec2(-FLT_MIN, 20 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int n = 0; n < palette.size(); n++)
            {
                const bool is_selected = (palette_selection == n);
                char buf[32];
                sprintf(buf, "%s", palette[n].name.c_str());
                if(ImGui::Selectable(buf, palette_selection == n))
                    palette_selection = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
    }
    ImGui::End();
}

void HistoryEditor(EditorCommands *editor_commands)
{
    ImGui::Begin("History");
    {
        if(ImGui::Button("U"))
            editor_commands->UndoCommand();
        ImGui::SameLine();
        if(editor_commands->current != -1)
            ImGui::Text("Last |  %s", editor_commands->history[editor_commands->current]->GetName().c_str());
        else
            ImGui::Text("No commands to undo.");
        ImGui::SameLine(ImGui::GetWindowWidth() - 40);
        if(ImGui::Button("R"))
            editor_commands->RedoCommand();

        if(ImGui::TreeNode("Entire")) {
            for (int n = editor_commands->history.size() - 1; n >= 0; n--)
            {
                char buf[32];
                sprintf(buf, "%s", editor_commands->history[n]->GetName().c_str());
                ImGui::Selectable(buf, editor_commands->current == n);
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

void LevelEditor(Level *level,
                 const vector<Object> &palette,
                 vector<Terrain> *terrains,
                 const vector<Skybox> &skyboxes,
                 const vector<Fog> &fogs,
                 EditorCommands *editor_commands
                )
{
    ImGui::Begin("Level Editor");
    {
        static char level_name[128] = DEFAULT_LEVEL;
        //ImGui::InputText("level", level_name, IM_ARRAYSIZE(level_name));
        if(ImGui::Button("Load")) {
            *level = LoadLevel(level_name, palette, *terrains, skyboxes, fogs);
            editor_commands->Clear();
        }
        ImGui::SameLine();
        if(ImGui::Button("Save")) {
            SaveLevel(*level);
        }

        if (ImGui::BeginListBox("## 1", ImVec2(-FLT_MIN, 10 * ImGui::GetTextLineHeightWithSpacing())))
        {
            for (int n = 0; n < level->objects.size(); n++)
            {
                const bool is_selected = (GlobalEditorState.selected == n);
                char buf[32];
                sprintf(buf, "%d | %s", n, level->objects[n].name.c_str());
                if(ImGui::Selectable(buf, GlobalEditorState.selected == n))
                    GlobalEditorState.selected = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }

        if(ImGui::TreeNode("Terrain"))
        {
            static int terrain_selection = 0;
            for (int n = 0; n < terrains->size(); n++)
            {
                char buf[32];
                sprintf(buf, "%s", terrains->at(n).name.c_str());
                if(ImGui::Selectable(buf, terrain_selection == n)) {
                    terrain_selection = n;
                    level->terrain = terrains->at(terrain_selection);
                }
            }

            ImGui::SliderFloat("scale", &level->terrain.y_scale, 16.0f, 256.0f);
            ImGui::SliderFloat("shift", &level->terrain.y_shift, -64.0f, 256.0f);
            if(ImGui::Button("update")) {
                level->terrain.Load(level->terrain.filename);
                level->terrain.GenerateVertices();
            }

            ImGui::ColorEdit3("Up Color", (float*)&level->terrain.up_color);
            ImGui::ColorEdit3("Side Color", (float*)&level->terrain.side_color);
            ImGui::SliderFloat("Color Threshold", &level->terrain.color_threshold, 0.0f, 1.0f);

            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Skybox")) {
            static int skybox_selection = -1;
            for (int n = 0; n < skyboxes.size(); n++)
            {
                char buf[32];
                sprintf(buf, "%s", skyboxes[n].name.c_str());
                if(ImGui::Selectable(buf, skybox_selection == n)) {
                    skybox_selection = n;
                    level->skybox = skyboxes[skybox_selection];
                }
            }
            if(ImGui::Button("Sunset")) {
                rotation_axis = vec3(0, 0, -1);
                rotation_degrees = 0.0f;
            }
            ImGui::SameLine();
            if(ImGui::Button("Sunrise")) {
                rotation_axis = vec3(0, 0, 1);
                rotation_degrees = -30.0f;
            }
            ImGui::SliderFloat("darkness", &skybox_darkness, 0.0f, 1.0f);

            ImGui::TreePop();
        }
        if(ImGui::TreeNode("Fog")) {
            static int fog_selection = -1;
            for (int n = 0; n < fogs.size(); n++)
            {
                char buf[32];
                sprintf(buf, "%s", fogs[n].name.c_str());
                if(ImGui::Selectable(buf, fog_selection == n)) {
                    fog_selection = n;
                    level->fog = fogs[fog_selection];
                }
            }

            ImGui::SliderFloat("min distance", &level->fog.min_distance, 10.0f, 500.0f);
            ImGui::SliderFloat("max distance", &level->fog.max_distance, 10.0f, 500.0f);
            ImGui::ColorEdit3("fog color", (float *)&(level->fog.color));
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

void ShadowEditor(Shadow *shadow)
{
    ImGui::Begin("Shadow Editor");
    {
        ImGui::SliderFloat("frustum", &shadow->shadow_frustum, 0.0f, 100.0f);
        ImGui::SliderFloat("near", &shadow->near_plane, -100.0f, 100.0f);
        ImGui::SliderFloat("far", &shadow->far_plane, 0.0f, 100.0f);
        ImGui::SliderFloat3("light pos", (float *)&shadow->light_pos, -10.0f, 10.0f);
    }
    ImGui::End();
}

void AudioSettings()
{
    ImGui::Begin("Audio");
    {
        static float music_volume = DEFAULT_MUSIC_VOLUME;
        static float sfx_volume = DEFAULT_SFX_VOLUME;
        if(ImGui::SliderFloat("Music", &music_volume, 0.0f, 1.0f))
            SetMusicVolume(music_volume);

        if(ImGui::SliderFloat("SFX", &sfx_volume, 0.0f, 1.0f))
            SetSfxVolume(sfx_volume);
    }
    ImGui::End();
}

void Editor(EditorCommands *editor_commands,
            RenderContext *context,
            const vector<Object> &palette,
            vector<Terrain> *terrains,
            const vector<Skybox> &skyboxes,
            const vector<Fog> &fogs,
            const Shaders &shaders,
            Level *level,
            Shadow *shadow
           )
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    {
        RenderContextEditor(context, shaders);
        LevelEditor(level, palette, terrains, skyboxes, fogs, editor_commands);
        ObjectEditor(&(level->objects), editor_commands);
        Palette(palette, &(level->objects));
        Generation(palette, &(level->objects), level->terrain);

        HistoryEditor(editor_commands);

        ShadowEditor(shadow);

        AudioSettings();

        DebugUI();

        ImGui::ShowDemoWindow();
    }
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif
