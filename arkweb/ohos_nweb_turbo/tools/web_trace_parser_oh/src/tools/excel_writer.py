import openpyxl
from openpyxl import Workbook, load_workbook
from openpyxl.utils import get_column_letter


#新建excel
def write_excel_xls(wb, path, sheet_name, value):
    # wb = Workbook()
    sheet = wb.create_sheet(sheet_name)
    for i in range(0, len(value[0])):
        sheet.cell(1, i + 1, value[0][i])
    wb.save(path)
    print("xls格式表格写入数据成功！")


def create_workbook_sheet_by_enum(wb, path, sheet_name, enum):
    sheet = wb.create_sheet(sheet_name)
    for tag in enum:
        sheet.cell(1, tag.value, tag.name)
    wb.save(path)
    print(f"xls create sheet {sheet_name} success！")


def create_workbook_sheet_by_list(wb, path, sheet_name, list):
    sheet = wb.create_sheet(sheet_name)
    for tag in list:
        sheet.cell(1, tag.value, tag.name)
    wb.save(path)
    print(f"xls create sheet {sheet_name} success！")


def write_excel_xls_append(path, sheet_name, x, y, time):
    workbook = openpyxl.load_workbook(path)  # 打开工作簿
    worksheet = workbook[sheet_name]  # 获取工作簿中所有表格中的的第一个表格

    #print('xls append:' + worksheet.cell_value(1, y))

    worksheet.cell(x, y, time)
    workbook.save(path)  # 保存工作簿


def create_sheet(workbook, path, sheet_name):
    workbook.create_sheet(sheet_name)
    workbook.save(path)


def write_excel_data(path, sheet_name, key_map, data_map):
    workbook = openpyxl.load_workbook(path)  # 打开工作簿
    worksheet = workbook[sheet_name]  # 获取工作簿中所有表格中的的第一个表格
    # 设置表头
    for index, key in enumerate(key_map):
        worksheet.cell(1, index + 1, key_map[key])
    # 写入数据
    for index, url in enumerate(data_map):
        for i, key in enumerate(key_map):
            # print(f"key: {key}, value: {getattr(data_map[url], key)}")
            worksheet.cell(index + 2, i + 1, getattr(data_map[url], key))

    workbook.save(path)


def write_excel_list(path, sheet_name, key_map, data_list):
    workbook = openpyxl.load_workbook(path)
    worksheet = workbook[sheet_name]
    # 设置表头
    for index, key in enumerate(key_map):
        worksheet.cell(1, index + 1, key_map[key])

    # 写入数据
    for index, unit in enumerate(data_list):
        for i, key in enumerate(key_map):
            value = getattr(unit, key)
            if isinstance(value, list):
                for j, url in enumerate(value):
                    worksheet.cell(index + 2, i + 1 + j, url)

            else:
                worksheet.cell(index + 2, i + 1, value)

    workbook.save(path)


def write_excel_map(
        path: str,
        sheet_name: str,
        key_map: dict[str, str],
        data_map: dict[str, str | float | int],
        start_line=1
):
    workbook = openpyxl.load_workbook(path)
    worksheet = workbook[sheet_name]
    # 设置表头
    for index, key in enumerate(key_map):
        worksheet.cell(start_line, index + 1, key_map[key])

    # 写入数据
    for i, k in enumerate(key_map):
        worksheet.cell(start_line + 1, i + 1, getattr(data_map, k))

    workbook.save(path)


def write_summary_excel(
        path: str,
        sheet_name: str,
        key_map: dict[str, str],
        data_map_list: list[dict[str, any]]
):
    wb = Workbook()
    create_sheet(wb, path, sheet_name)
    workbook = openpyxl.load_workbook(path)
    worksheet = workbook[sheet_name]
    for index, key in enumerate(key_map):
        worksheet.cell(1, index + 1, key_map[key])

    for index, data_map in enumerate(data_map_list):
        print(f'data_map: {data_map}')
        for j, key in enumerate(key_map):
            worksheet.cell(index + 2, j + 1, data_map[key])

    workbook.save(path)
