Sub RemoveOLE()
'
' RemoveOLE 宏
'
'
Dim MyBk As Bookmark
For Each MyBk In ActiveDocument.Bookmarks
    MyBk.Delete
Next
End Sub
