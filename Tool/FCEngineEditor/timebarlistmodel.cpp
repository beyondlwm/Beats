#include "stdafx.h"
#include "timebarlistmodel.h"

static int my_sort_reverse( int *v1, int *v2 )
{
    return *v2-*v1;
}

static int my_sort( int *v1, int *v2 )
{
    return *v1-*v2;
}

#define INITIAL_NUMBER_OF_ITEMS 1

TimeBarListModel::TimeBarListModel()
: wxDataViewVirtualListModel( INITIAL_NUMBER_OF_ITEMS )
{

}

TimeBarListModel::~TimeBarListModel()
{

}

void TimeBarListModel::DeleteItem( int index )
{
    unsigned int row = index;

    if (row < m_textColValues.GetCount())
    {
        m_textColValues.RemoveAt( row );
        RowDeleted( row );
    }
}

void TimeBarListModel::DeleteItems( const wxDataViewItemArray &items )
{
    wxArrayInt rows;
    for (unsigned i = 0; i < items.GetCount(); i++)
    {
        unsigned int row = GetRow( items[i] );
        if (row < m_textColValues.GetCount())
            rows.Add( row );
    }

    if (rows.GetCount() > 0)
    {
        // Sort in descending order so that the last
        // row will be deleted first. Otherwise the
        // remaining indeces would all be wrong.
        rows.Sort( my_sort_reverse );
        for (unsigned i = 0; i < rows.GetCount(); i++)
        {
            m_textColValues.RemoveAt( rows[i] );
            m_bview.RemoveAt( rows[i] );
            m_block.RemoveAt( rows[i] );
        }
        // This is just to test if wxDataViewCtrl can
        // cope with removing rows not sorted in
        // descending order
        rows.Sort( my_sort );
        RowsDeleted( rows );
    }
}

void TimeBarListModel::AddItem( const wxString text, bool visible, bool islock )
{
    m_textColValues.push_back(text);
    m_bview.push_back(visible);
    m_block.push_back(islock);
    Reset(m_textColValues.Count());
}

void TimeBarListModel::GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const
{
    if ((row < m_textColValues.GetCount()))
    {
        switch ( col )
        {
        case Col_EditableText:
            variant = m_textColValues[ row ];
            break;

        case Col_IsVisible:
            variant = m_bview[row];
            break;

        case Col_Islocked:
            variant = m_block[row];
            break;

        case Col_Max:
            wxFAIL_MSG( _T("invalid column") );
            break;

        default:
            break;
        }
    }
}

bool TimeBarListModel::SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col )
{
    if (row < m_textColValues.GetCount())
    {
        switch ( col )
        {
        case Col_EditableText:
            m_textColValues[row] = variant.GetString();
            break;

        case Col_IsVisible:
            m_bview[row] = variant.GetChar();
            break;

        case Col_Islocked:
            m_block[row] = variant.GetChar();
            break;

        case Col_Max:
        default:
            wxFAIL_MSG( _T("invalid column") );
            break;
        }
    }
    return false;
}

unsigned int TimeBarListModel::GetColumnCount() const
{
    return Col_Max;
}

wxString TimeBarListModel::GetColumnType( unsigned int /*col*/ ) const
{
    return wxT("string");
}
