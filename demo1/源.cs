public DataTable Get ? (string? )
{
	DataTable dt = null;
	string sql = null;
	SqlParameter[] cmdParms = {
        new SqlParameter("@?", ? ),
    };//这样比直接用传参更安全，因为查看sql语句时得到的会是@string而不是直接看到了具体的值
	sql = "select * from tableName";
	dt = SQLHelper.GetDataTable(sql, CommandType.Text, cmdParms);
	if (dt == null)
	{
		return TableTools.CreateTable("error", "没有操作数据库进行查询，返回值未实例化，为null！");
	}
	else if (dt.Rows.Count == 0)
	{
		return TableTools.CreateTable("error", "表格无数据！");
	}
	return dt;
}