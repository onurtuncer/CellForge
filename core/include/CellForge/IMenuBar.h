

namespace CellForge{

        class IMenuBar
    {
    public:
        virtual ~IMenuBar() = default;

        virtual void AddMenu(const std::string& name) = 0;

        virtual void AddAction(
            const std::string& menu,
            const std::string& actionName,
            std::function<void()> callback) = 0;
    };

}