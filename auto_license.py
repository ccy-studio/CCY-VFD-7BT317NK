'''
Description: 
Blog: saisaiwa.com
Author: ccy
Date: 2023-08-22 16:36:04
LastEditTime: 2023-08-22 16:51:22
'''
import os

# 要添加的版权声明
copyright_notice = """
/***********************************************************************************************
 * 版权声明：
 * 本源代码的版权归 [saisaiwa] 所有。
 *
 * 未经版权所有者明确授权，不得将本代码的任何部分用于商业用途，包括但不限于出售、出租、许可或发布。
 * 仅限个人学习、研究、非盈利性用途下使用。如果您有其他用途的需求，请联系 [yustart@foxmail.com] 进行授权。
 *
 * 在遵循以下条件的情况下，您可以自由修改、使用和分发本代码：
 * - 您必须保留此版权声明的所有内容。
 * - 您不得用于任何违法、滥用、恶意的活动。
 * - 您的使用不应损害任何个人、组织或作者的声誉。
 *
 * 作者不承担因使用本代码而产生的任何担保责任。作者对因使用本代码所导致的任何直接或间接损失不承担责任。
 * Github: https://github.com/ccy-studio/CCY-VFD-7BT317NK
 ***********************************************************************************************/
"""

# 设置文件后缀名
allowed_extensions = ['.cpp', '.h']

# 遍历当前目录及其子目录下的文件
for root, dirs, files in os.walk('lib'):
    for file in files:
        # 仅处理指定的文件后缀名
        if any(file.endswith(ext) for ext in allowed_extensions):
            file_path = os.path.join(root, file)
            # 读取原文件内容
            with open(file_path, 'r', encoding='utf-8') as f:
                file_content = f.read()
            if not file_content.strip().startswith("/****************************"):
                # 将版权声明插入到文件头部
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(copyright_notice.strip() + '\n\n' + file_content)
                print(f"已在文件 {file_path} 中添加版权声明。")
            else:
                print(f"文件 {file_path} 已包含版权声明，忽略添加。")
