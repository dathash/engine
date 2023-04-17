// Author: Alex Hartford
// Program: Engine
// File: Command
// Date: March 2023

#ifndef COMMAND_H
#define COMMAND_H

struct Command
{
    virtual string GetName() = 0;
    virtual void PrintDetails() = 0;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
    virtual ~Command() = default;
};

struct NullCommand : public Command
{
    string GetName() { return "Null Command."; }
    void PrintDetails() { cout << "Null Command.\n"; }

    void Execute() {
        cout << "Null Command Execute\n";
    }
    void Undo() {
        cout << "Null Command Undo\n";
    }
};

struct MoveObjectCommand : public Command
{
    string GetName() { return string("Move Object: \"") + 
                       object->name + string("\". "); }
    void PrintDetails() { cout << "Move Object " << object->name 
                               << " from \n"
                               << start 
                               << "to \n" 
                               << end; }

    Object *object;
    mat4 start;
    mat4 end;

    MoveObjectCommand(Object *object_in, const mat4 &start_in, const mat4 &end_in)
    : object(object_in), start(start_in), end(end_in) {}

    void Execute() {
        vec3 scl;
        quat rot;
        vec3 pos;
        vec3 skew;
        vec4 perspective;
        
        glm::decompose(end, scl, rot, pos, 
                       skew, perspective);

        object->scale_factor = scl.x;
        object->orientation = rot;
        object->position = pos;
        object->UpdateMatrix();
    }
    void Undo() {
        vec3 scl;
        quat rot;
        vec3 pos;
        vec3 skew;
        vec4 perspective;
        
        glm::decompose(start, scl, rot, pos, 
                       skew, perspective);

        object->scale_factor = scl.x;
        object->orientation = rot;
        object->position = pos;
        object->UpdateMatrix();
    }
};

struct DestroyObjectCommand : public Command
{
    string GetName() { return string("Destroy Object: \"") + copy.name + string("\"."); }
    void PrintDetails() { cout << "Destroy Object " << copy.name << "\n"; }

    vector<Object> *level;
    int index;
    Object copy;

    DestroyObjectCommand(vector<Object> *level_in, int index_in)
    : level(level_in), index(index_in) {}

    void Execute() {
        assert(index < level->size());
        copy = Object(level->at(index));
        level->erase(level->begin() + index);
        GlobalEditorState.selected = -1;
    }
    void Undo() {
        level->insert(level->begin() + index, Object(copy));
        GlobalEditorState.selected = index;
    }
};

struct AddObjectCommand : public Command
{
    string GetName() { return string("Add Object: \"") + original.name + string("\"."); }
    void PrintDetails() { cout << "Add Object " << original.name << "\n"; }

    vector<Object> *level;
    Object original;

    AddObjectCommand(vector<Object> *level_in, const Object &original_in)
    : level(level_in), original(original_in) {}

    void Execute() {
        level->push_back(Object(original));
        GlobalEditorState.selected = level->size() - 1;
    }
    void Undo() {
        level->pop_back();
        GlobalEditorState.selected = -1;
    }
};

struct EditorCommands
{
    vector<shared_ptr<Command>> history = {};
    int current = -1;

    shared_ptr<Command> destroy;
    shared_ptr<Command> copy;

    EditorCommands()
    {
        destroy = make_shared<NullCommand>();
        copy = make_shared<NullCommand>();
    }

    void Clear()
    {
        history = {};
        current = -1;
    }

    void UpdateCommands(Level *level)
    {
        static Object *selected;
        if(GlobalEditorState.selected != -1) {
            selected = &(level->objects[GlobalEditorState.selected]);
        }
        else
            selected = nullptr;

        if(selected) {
            destroy = make_shared<DestroyObjectCommand>(&(level->objects), GlobalEditorState.selected);
            copy    = make_shared<AddObjectCommand>(&(level->objects), *selected);
        }
        else {
            destroy = make_shared<NullCommand>();
            copy = make_shared<NullCommand>();
        }
    }

    void HandleInput()
    {
        if(input.destroy)
        {
            input.destroy = false;

            destroy->Execute();
            AddToHistory(destroy);
        }
        if(input.copy) {
            input.copy = false;

            copy->Execute();
            AddToHistory(copy);
        }

        // ================== Meta Bookkeeping =================================
        if(input.undo)
        {
            input.undo = false;
            UndoCommand();
        }
        if(input.redo)
        {
            input.redo = false;
            RedoCommand();
        }
    }

    void AddToHistory(shared_ptr<Command> executed)
    {
        assert(executed);

        if(current == history.size() - 1)
        {
            history.push_back(executed);
            ++current;
        }
        else // Overwrite the future
        {
            history.resize(current + 1);
            history.push_back(executed);
            ++current;
        }
    }

    void UndoCommand() { 
        if(current >= 0) {
            history[current]->Undo();
            --current;
        }
    }

    void RedoCommand() { 
        if(current < (int)history.size() - 1) {
            ++current;
            history[current]->Execute();
        }
    }
};

#endif
