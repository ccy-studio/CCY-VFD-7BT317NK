package org.example;

import cn.hutool.core.io.FileUtil;
import cn.hutool.core.util.ArrayUtil;
import cn.hutool.core.util.StrUtil;

import java.io.File;
import java.util.List;

/**
 * @Description:
 * @Author: Chen Ze Deng
 * @Date: 2023/7/12 16:25
 * @Version：1.0
 */
public class VFD_FontGenerator {

    public static void main(String[] args) throws Exception {
        File file = new File("C:\\Users\\83503\\Desktop\\字库定义.txt");
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

            StringBuffer sb = new StringBuffer();
            sb.append("{");
            sb.append("{")
                    .append(hex1)
                    .append(",")
                    .append(hex2)
                    .append(",")
                    .append(hex3)
                    .append("},")
                    .append("'")
                    .append(num)
                    .append("'");
            sb.append("},");
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
        return "0x" + Integer.toString(i, 16);
    }
}
