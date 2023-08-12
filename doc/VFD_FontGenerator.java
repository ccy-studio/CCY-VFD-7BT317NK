/*
 * @Description: 
 * @Author: chenzedeng
 * @Date: 2023-07-28 21:57:30
 * @LastEditTime: 2023-08-12 15:56:45
 */
package cn.yuchen.test;

import cn.hutool.core.io.FileUtil;
import cn.hutool.core.util.ArrayUtil;
import cn.hutool.core.util.StrUtil;

import java.io.File;
import java.util.List;

/**
 * @author chenzedeng
 * @date 2023/3/28 21:47
 */
public class Test1 {

    public static void main(String[] args) throws Exception {
        File file = new File("/Users/chenzedeng/CodeWorkspace/Project/Embedded/Arduino_Work/CCY-VFD-7BT317NK/doc/字库定义.txt");
        List<String> lines = FileUtil.readLines(file, "utf-8");
        int sum = 0;
        for (int i = 0; i < lines.size(); i += 4) {
            String num = lines.get(i).trim();
            if (StrUtil.isBlank(num)) {
                continue;
            }
            String[] ps = lines.get(i + 1).trim()
                    .replaceAll("1-", "")
                    .trim().replaceAll("p", "")
                    .trim()
                    .split(",");
            byte[] bytes = new byte[24];
            for (String p : ps) {
                Integer index = Integer.valueOf(p) - 1;
                bytes[index] = 1;
            }
            byte[][] split = ArrayUtil.split(bytes, 8);
            String hex1 = toHex(split[0]);
            String hex2 = toHex(split[1]);
            String hex3 = toHex(split[2]);

            String hex32 = "0x" + hex1 + hex2 + hex3;

            StringBuffer sb = new StringBuffer();
            sb.append(hex32);
            sb.append(", // ASCII:");
            sb.append(num);
            sb.append(" index:");
            sb.append(sum);
            System.out.println(sb);
            sum++;
        }
        System.out.println("总数: " + sum);
    }

    public static String toHex(byte[] b) {
        StringBuffer sb = new StringBuffer();
        for (byte b1 : ArrayUtil.reverse(b)) {
            sb.append(String.valueOf(b1));
        }
        int i = Integer.parseInt(sb.toString(), 2);
        String hex = Integer.toString(i, 16);
        if (hex.length() == 1) {
            return "0" + hex;
        }
        return hex;
    }

}
