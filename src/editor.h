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
        ImGui::SliderFloat3("light dir", (float *)&(context->light_direction), -1.0f, 1.0f);
        ImGui::ColorEdit4("color", (float *)&(context->default_color));
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
        ImGui::Checkbox("shadow", &(context->shadow));

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
    }
    ImGui::End();
}

void Palette(const vector<Object> &palette,
             vector<Object> *level)
{
    static int palette_selection = -1;
    ImGui::Begin("Palette");
    {
        if(ImGui::Button("Add"))
        {
            if(palette_selection != -1)
                level->push_back(Object(palette[palette_selection]));
                GlobalEditorState.selected = level->size() - 1;
        }
        for (int n = 0; n < palette.size(); n++)
        {
            char buf[32];
            sprintf(buf, "%s", palette[n].name.c_str());
            if(ImGui::Selectable(buf, palette_selection == n))
                palette_selection = n;
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
        ImGui::InputText("level", level_name, IM_ARRAYSIZE(level_name));
        if(ImGui::Button("Load")) {
            *level = LoadLevel(level_name, palette, *terrains, skyboxes, fogs);
            editor_commands->Clear();
        }
        ImGui::SameLine();
        if(ImGui::Button("Save")) {
            SaveLevel(*level);
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
            ImGui::TreePop();
        }

        for(int n = 0; n < level->objects.size(); ++n) {
            char buf[32];
            sprintf(buf, "%d | %s", n, level->objects[n].name.c_str());
            if(ImGui::Selectable(buf, GlobalEditorState.selected == n))
                GlobalEditorState.selected = n;
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

void LineEditor(Line *line, Line *smooth)
{
    ImGui::Begin("Swordfish");
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

void LineEditor2(Line *line, Line *smooth)
{
    ImGui::Begin("Hornet");
    {
        static int line_selection = 0;
        if(ImGui::BeginListBox("points_2"))
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

void Editor(EditorCommands *editor_commands,
            RenderContext *context,
            const vector<Object> &palette,
            vector<Terrain> *terrains,
            const vector<Skybox> &skyboxes,
            const vector<Fog> &fogs,
            const Shaders &shaders,
            Level *level,
            Shadow *shadow,
            Line *line,
            Line *line2,
            Line *smooth,
            Line *smooth2
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
        LineEditor(line, smooth);
        LineEditor2(line2, smooth2);

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
