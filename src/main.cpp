import engine;

int main(int argc, char** argv)
{
    Engine& engine = Engine::get();
    engine.init();
    engine.run();
    return 0;
}