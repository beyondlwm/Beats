#ifndef __MYLISTMODEL_H
#define __MYLISTMODEL_H

#include <wx/dynarray.h>
#include <wx/dataview.h>

WX_DEFINE_ARRAY_CHAR(char, wxArrayChar);
class TimeBarListModel: public wxDataViewVirtualListModel
{
public:
    enum
    {
        Col_EditableText,
        Col_IsVisible,
        Col_Islocked,
        Col_Max
    };

    TimeBarListModel();
    virtual ~TimeBarListModel();

    // helper methods to change the model
    void DeleteItem( int index );
    void DeleteItems( const wxDataViewItemArray &items );
    void AddItem( const wxString text, bool visible, bool islock );

    // implementation of base class virtuals to define model
    virtual unsigned int    GetColumnCount() const;
    virtual wxString        GetColumnType( unsigned int col ) const;
    virtual void            GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const;
    virtual bool            SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col );

private:

    wxArrayString    m_textColValues;
    wxArrayString    m_iconColValues;
    wxArrayChar      m_bview;
    wxArrayChar      m_block;
    wxBitmap         m_BmpV;
    wxBitmap         m_BmpL;
};


#endif