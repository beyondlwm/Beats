#ifndef GUI_ANIMATION_H__INCLUDE
#define GUI_ANIMATION_H__INCLUDE

namespace FCGUI
{
    class KeyFrame
    {
    public:
        typedef std::vector<kmScalar> ValueList;

        KeyFrame(kmScalar position);

        kmScalar Position() const;

        void SetValueCount(size_t size);
        size_t ValueCount() const;

        void SetValue(size_t index, kmScalar value);
        void DeleteValue(size_t index);
        kmScalar Value(size_t index) const;
        const ValueList &Values() const;

    private:
        kmScalar _position;
        ValueList _values;
    };

}

#endif // !GUI_ANIMATION_H__INCLUDE
