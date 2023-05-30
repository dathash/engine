// Author: Alex Hartford
// Program: Engine
// File: Render
// Date: March 2023

#ifndef RENDER_H
#define RENDER_H

struct RenderContext
{
    unsigned int shader_index;
    vec3 light_direction;
    vec4 default_color;
    bool use_default_color;
    vec4 background_color;

    float brightness;
    float opacity;

    float cel_threshold_high;
    float cel_threshold_mid;
    float cel_threshold_low;
    bool draw_lines;

    bool skybox;

    bool processing;
    bool water;
    bool shadow;
};

void RenderObjectPreview(const RenderContext &context,
                         const Object &object,
                         const Shader &shader)
{
    // Render tiny guy
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    glViewport(-30, 0, 200, 200);

    mat4 view = lookAt(vec3(0.0f, 0.0f, 2.5f),
                  vec3(0.0f, 0.0f, -1.0f),
                  vec3(0.0f, 1.0f, 0.0f));
    mat4 projection = perspective(1.0f,
                      (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
                      0.1f, 1000.0f);

    // Textures
    shader.bind();
    {
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        shader.setVec3("light_direction", vec3(0.0f, -0.5f, -1.0f));

        shader.setVec4("color", context.default_color);
        shader.setFloat("brightness", context.brightness);
        shader.setFloat("opacity", context.opacity);

        shader.setFloat("cel_threshold_high", context.cel_threshold_high);
        shader.setFloat("cel_threshold_mid", context.cel_threshold_mid);
        shader.setFloat("cel_threshold_low", context.cel_threshold_low);

        if(GlobalEditorState.selected != -1)
            object.DrawRotatingAtOrigin(shader);
    }
    shader.unbind();
}

static float rotation_degrees = 0.0f;
static vec3 rotation_axis = vec3(0, 0, -1);
static float skybox_darkness = 0.0f;
void RenderSkybox(const Skybox &skybox,
                  const Shader &cube_shader,
                  const mat4 &projection,
                  const mat4 &view)
{
    cube_shader.bind();
    {
        cube_shader.setMat4("projection", projection);
        // NOTE: Remove translation part of view matrix
        cube_shader.setMat4("view", mat4(mat3(view)));

        mat4 model = mat4(1.0f);
        model = rotate(model, radians(rotation_degrees), rotation_axis);
        cube_shader.setMat4("model", model);

        cube_shader.setFloat("skybox_darkness", skybox_darkness);

        glStencilFunc(GL_ALWAYS, 201, -1);
        skybox.Draw(cube_shader);
    }
    cube_shader.unbind();
}


void RenderHeightmap(const RenderContext &context,
                     const Terrain &terrain,
                     const Shadow &shadow,
                     const Fog &fog,
                     const Shader &heightmap_shader,
                     const mat4 &projection,
                     const mat4 &view,
                     const vec4 &clip_plane)
{
    heightmap_shader.bind();
    {
        heightmap_shader.setMat4("projection", projection);
        heightmap_shader.setMat4("view", view);

        heightmap_shader.setVec3("light_direction", context.light_direction);
        heightmap_shader.setFloat("brightness", context.brightness);
        heightmap_shader.setFloat("opacity", context.opacity);

        heightmap_shader.setFloat("cel_threshold_high", context.cel_threshold_high);
        heightmap_shader.setFloat("cel_threshold_mid", context.cel_threshold_mid);
        heightmap_shader.setFloat("cel_threshold_low", context.cel_threshold_low);

        heightmap_shader.setFloat("min_fog_distance", fog.min_distance);
        heightmap_shader.setFloat("max_fog_distance", fog.max_distance);
        heightmap_shader.setVec3("fog_color", fog.color);

        heightmap_shader.setVec3("up_color", terrain.up_color);
        heightmap_shader.setVec3("side_color", terrain.side_color);
        heightmap_shader.setInt("terrain_size", terrain.width);
        heightmap_shader.setFloat("heightmap_color_threshold", terrain.color_threshold);

        heightmap_shader.setVec4("clip_plane", clip_plane);

        heightmap_shader.setMat4("light_projection", shadow.GetProjectionMatrix());
        heightmap_shader.setMat4("light_view", shadow.GetViewMatrix());

        heightmap_shader.setInt("shadow_map", 5); // TODO: I'm doing this to avoid the mesh textures... is this necessary? i don't know enough yet...
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, shadow.depth_buffer);

        glStencilFunc(GL_ALWAYS, 200, -1);
        terrain.Draw(heightmap_shader);
    }
    heightmap_shader.unbind();
}

void RenderObjects(const RenderContext &context,
                   const vector<Object> &objects,
                   const Shadow &shadow,
                   const Fog &fog,
                   const Shader &shader,
                   const mat4 &projection,
                   const mat4 &view,
                   const vec4 &clip_plane)
{
    shader.bind();
    {
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        shader.setVec3("light_direction", context.light_direction);

        shader.setVec4("color", context.default_color);
        shader.setInt("use_default_color", context.use_default_color);
        shader.setFloat("brightness", context.brightness);

        shader.setFloat("opacity", context.opacity);

        shader.setFloat("cel_threshold_high", context.cel_threshold_high);
        shader.setFloat("cel_threshold_mid", context.cel_threshold_mid);
        shader.setFloat("cel_threshold_low", context.cel_threshold_low);

        shader.setFloat("min_fog_distance", fog.min_distance);
        shader.setFloat("max_fog_distance", fog.max_distance);
        shader.setVec3("fog_color", fog.color);

        shader.setVec4("clip_plane", clip_plane);

        shader.setMat4("light_projection", shadow.GetProjectionMatrix());
        shader.setMat4("light_view", shadow.GetViewMatrix());

        shader.setInt("shadow_map", 5); // TODO: I'm doing this to avoid the mesh textures... is this necessary? i don't know enough yet...
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, shadow.depth_buffer);

        for(int i = 0; i < objects.size(); i++) {
            glStencilFunc(GL_ALWAYS, i + 1, -1);

            GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
            glDrawBuffers(2, drawBuffers);

            if(objects[i].animator.current_animation) {
                auto transforms = objects[i].animator.final_bone_matrices;
                for (int i = 0; i < transforms.size(); ++i) {
                    shader.setMat4("bones[" + to_string(i) + "]", transforms[i]);
                }
                shader.setInt("animated", 1);
            }
            else {
                shader.setInt("animated", 0);
            }

            objects[i].Draw(shader);

            if(i == GlobalEditorState.selected) {
                shader.setFloat("highlight_amount", sin(5*Time()));
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                objects[i].Draw(shader);
            }
            shader.setFloat("highlight_amount", 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
    shader.unbind();
}

// Renders the given framebuffer as a texture on a screen-space quad.
void RenderShadowFramebufferToScreen(const Shader &quad_shader,
                                     const Shadow &shadow)
{
    // Draw to screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    quad_shader.bind();
    {
        quad_shader.setInt("color_tex", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadow.depth_buffer);

        glBindVertexArray(shadow.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    quad_shader.unbind();
    glEnable(GL_DEPTH_TEST);
}

void RenderLine(const Line &line,
                const Shader &shader,
                const vec4 &color,
                const mat4 &projection,
                const mat4 &view)
{
    shader.bind();
    {
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        shader.setVec4("color", color);

        line.Draw(shader);
    }
    shader.unbind();
}

// Renders the given framebuffer as a texture on a screen-space quad.
void RenderFramebufferToScreen(const Shader &quad_shader,
                               const Framebuffer &framebuffer)
{
    // Draw to screen.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    quad_shader.bind();
    {
        quad_shader.setInt("color_tex", 0);
        quad_shader.setInt("position_tex", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, framebuffer.colorbuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, framebuffer.depthbuffer);

        glBindVertexArray(framebuffer.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    quad_shader.unbind();
    glEnable(GL_DEPTH_TEST);
}

void RenderScene(const RenderContext &context,
                 const Shaders &shaders,
                 const Level &level,
                 const Shadow &shadow,
                 const mat4 &projection,
                 const mat4 &view,
                 const vec4 &clip_plane)
{
    glClearColor(context.background_color.r, context.background_color.g, context.background_color.b, 1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    RenderHeightmap(context, level.terrain, shadow, level.fog,
                    shaders.heightmap_shader,
                    projection, view, clip_plane);
    RenderObjects(context, level.objects, shadow, level.fog,
                  shaders.list[context.shader_index],
                  projection, view, clip_plane);

    if (GlobalMode == EDITOR && GlobalEditorState.selected != -1) {
        if(context.draw_lines) {
            RenderLine(level.objects[GlobalEditorState.selected].line, shaders.line_shader, vec4(1.0f, 1.0f, 0.0f, 1.0f), projection, view);
            RenderLine(level.objects[GlobalEditorState.selected].path, shaders.line_shader, vec4(1.0f, 0.0f, 1.0f, 1.0f), projection, view);
        }
    }

    if(context.skybox)
        RenderSkybox(level.skybox, shaders.cube_shader, projection, view);

    /*
    if(GlobalMode == GAME && GlobalEditorState.selected != -1)
        RenderObjectPreview(context, level.objects[GlobalEditorState.selected], shaders.list[context.shader_index]);
    */
}

void RenderWater(const Shader &water_shader,
                 const Water &water,
                 const mat4 &projection,
                 const mat4 &view)
{
    water_shader.bind();
    {
        water_shader.setMat4("projection", projection);
        water_shader.setMat4("view", view);

        water_shader.setInt("reflection_tex", 0);
        water_shader.setInt("refraction_tex", 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, water.reflection_buffer.colorbuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, water.refraction_buffer.colorbuffer);

        glStencilFunc(GL_ALWAYS, 201, -1);
        water.Draw(water_shader);
    }
    water_shader.unbind();
}

void Render(const RenderContext &context,
            const Shaders &shaders,
            const Level &level,
            const Water &water,
            const Shadow &shadow,
            const Framebuffer &framebuffer
           )
{
    camera.projection = camera.GetProjectionMatrix();
    camera.view = camera.GetViewMatrix();
    mat4 projection = camera.GetProjectionMatrix();
    mat4 view = camera.GetViewMatrix();

    if(camera.focus_index != -1) {
        view = lookAt(camera.position, level.objects[camera.focus_index].position, vec3(0, 1, 0));
    }

    // No Framebuffer shenanigans
    if(!context.processing)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        RenderScene(context, shaders, level, shadow,
                    projection, view,
                    CLIP_NOTHING);
        return;
    }

    // Water Pass
    if(context.water)
    {
        // Reflection (From camera under the sea)
        glBindFramebuffer(GL_FRAMEBUFFER, water.reflection_buffer.FBO);
        glViewport(0, 0, water.reflection_buffer.width, water.reflection_buffer.height);
        float invert_distance = 2 * (camera.position.y - water.height);
        mat4 invert_view = camera.GetInvertedViewMatrix(invert_distance);
        RenderScene(context, shaders, level, shadow,
                    projection, invert_view,
                    vec4(0.0f, 1.0f, 0.0f, -water.height)); // Above water
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Refraction
        glBindFramebuffer(GL_FRAMEBUFFER, water.refraction_buffer.FBO);
        glViewport(0, 0, water.refraction_buffer.width, water.refraction_buffer.height);
        RenderScene(context, shaders, level, shadow,
                    projection, view,
                    vec4(0.0f, -1.0f, 0.0f, water.height)); // Below water

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //RenderFramebufferToScreen(shaders.quad_shader, water.refraction_buffer);
    }

    // Shadow Pass
    if(context.shadow)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
        glViewport(0, 0, shadow.SHADOW_WIDTH, shadow.SHADOW_HEIGHT);
        glClear(GL_DEPTH_BUFFER_BIT);

        const Shader &depth_shader = shaders.depth_shader;
        depth_shader.bind();
        {
            depth_shader.setMat4("projection", shadow.GetProjectionMatrix());
            depth_shader.setMat4("view", shadow.GetViewMatrix());

            level.terrain.Draw(depth_shader);
            for(int i = 0; i < level.objects.size(); ++i) {
                if(level.objects[i].animator.current_animation) {
                    auto transforms = level.objects[i].animator.final_bone_matrices;
                    for (int i = 0; i < transforms.size(); ++i) {
                        depth_shader.setMat4("bones[" + to_string(i) + "]", transforms[i]);
                    }
                    depth_shader.setInt("animated", 1);
                }
                else {
                    depth_shader.setInt("animated", 0);
                }

                level.objects[i].Draw(depth_shader);
            }
        }
        depth_shader.unbind();

        if(GlobalDepthBufferDebug)
        {
            RenderShadowFramebufferToScreen(shaders.quad_shader, shadow);
            return;
        }
    }

    // Final Render Pass
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.FBO);
    glEnable(GL_DEPTH_TEST); // ? remove
    glViewport(0, 0, framebuffer.width, framebuffer.height);

    RenderScene(context, shaders, level, shadow,
                projection, view,
                CLIP_NOTHING);
    RenderWater(shaders.water_shader, water, projection, view);

    if(context.processing)
        RenderFramebufferToScreen(shaders.processing_shader, framebuffer);
    else
        RenderFramebufferToScreen(shaders.quad_shader, framebuffer);
}

#endif
