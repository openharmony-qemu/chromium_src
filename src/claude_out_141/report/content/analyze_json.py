#!/usr/bin/env python3
import json
import os
from collections import defaultdict, Counter

def analyze_json_file(filepath):
    """分析单个JSON文件并提取关键信息"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except Exception as e:
        return {"error": str(e)}

    filename = os.path.basename(filepath)
    result = {
        "filename": filename,
        "file_path": filepath,
        "total_hunks": 0,
        "resolved_hunks": 0,
        "unresolved_hunks": 0,
        "success_rate": "0%",
        "risk_levels": [],
        "conflict_types": [],
        "resolution_status": "unknown",
        "main_features": []
    }

    # 尝试提取不同格式的统计信息
    # 格式1: 英文格式 (.summary, .hunks_analysis)
    if 'summary' in data:
        result.update({
            "total_hunks": data.get('summary', {}).get('total_hunks', 0),
            "resolved_hunks": data.get('summary', {}).get('resolved_hunks', 0),
            "unresolved_hunks": data.get('summary', {}).get('unresolved_hunks', 0)
        })

    # 格式2: 中文格式 (.处理总结, .总体统计, .处理统计)
    if '处理总结' in data:
        result.update({
            "total_hunks": data['处理总结'].get('总hunk数量', 0),
            "resolved_hunks": data['处理总结'].get('成功解决的hunk数量', 0),
            "unresolved_hunks": data['处理总结'].get('未能解决的hunk数量', 0)
        })
    elif '总体统计' in data:
        result.update({
            "total_hunks": data['总体统计'].get('总hunk数量', 0),
            "resolved_hunks": data['总体统计'].get('成功解决的hunk数量', 0),
            "unresolved_hunks": data['总体统计'].get('未能解决的hunk数量', 0)
        })
    elif '处理统计' in data:
        result.update({
            "total_hunks": data['处理统计'].get('总hunk数量', 0),
            "resolved_hunks": data['处理统计'].get('成功解决的hunk数量', 0),
            "unresolved_hunks": data['处理统计'].get('未能解决的hunk数量', 0)
        })

    # 格式3: 直接字段
    if 'total_hunks' in data:
        result["total_hunks"] = data.get('total_hunks', 0)
        result["resolved_hunks"] = data.get('resolved_hunks', 0)
        result["unresolved_hunks"] = data.get('unresolved_hunks', 0)

    # 计算成功率
    if result["total_hunks"] > 0:
        success_rate = (result["resolved_hunks"] / result["total_hunks"]) * 100
        result["success_rate"] = f"{success_rate:.1f}%"

    # 提取风险等级
    if 'hunks_analysis' in data:
        for hunk in data['hunks_analysis']:
            risk = hunk.get('risk_level', 'N/A')
            result["risk_levels"].append(risk)
    elif 'hunks详情' in data:
        for hunk in data['hunks详情']:
            risk = hunk.get('风险等级') or hunk.get('风险分析', {}).get('风险等级', 'N/A')
            result["risk_levels"].append(risk)

    # 提取冲突类型
    if 'hunks_analysis' in data:
        for hunk in data['hunks_analysis']:
            conflict = hunk.get('conflict_cause', 'N/A')
            result["conflict_types"].append(conflict)
    elif 'hunks详情' in data:
        for hunk in data['hunks详情']:
            conflict = hunk.get('冲突原因', 'N/A')
            result["conflict_types"].append(conflict)

    # 判断解决状态
    if result["resolved_hunks"] == result["total_hunks"] and result["total_hunks"] > 0:
        result["resolution_status"] = "全部解决"
    elif result["resolved_hunks"] > 0:
        result["resolution_status"] = "部分解决"
    elif result["total_hunks"] == 0:
        result["resolution_status"] = "无冲突"
    else:
        result["resolution_status"] = "未解决"

    # 提取主要特性/功能
    if 'technical_assessment' in data:
        result["main_features"].append(data['technical_assessment'].get('modification_purpose', ''))

    return result

def main():
    # 获取当前目录下所有JSON文件
    json_files = [f for f in os.listdir('.') if f.endswith('.json')]

    print(f"开始分析 {len(json_files)} 个JSON文件...")
    print("=" * 80)

    all_results = []

    for json_file in json_files:
        result = analyze_json_file(json_file)
        if 'error' not in result:
            all_results.append(result)

    # 汇总统计
    print("\n📊 汇总统计信息")
    print("=" * 80)
    print(f"总文件数量: {len(all_results)}")

    total_hunks = sum(r['total_hunks'] for r in all_results)
    resolved_hunks = sum(r['resolved_hunks'] for r in all_results)
    unresolved_hunks = sum(r['unresolved_hunks'] for r in all_results)

    print(f"总Hunk数量: {total_hunks}")
    print(f"已解决Hunk数量: {resolved_hunks}")
    print(f"未解决Hunk数量: {unresolved_hunks}")
    print(f"整体解决成功率: {(resolved_hunks/total_hunks*100):.1f}%" if total_hunks > 0 else "N/A")

    # 状态分布
    print("\n📈 解决状态分布")
    print("-" * 40)
    status_counter = Counter(r['resolution_status'] for r in all_results)
    for status, count in status_counter.most_common():
        print(f"{status}: {count} 个文件")

    # 风险等级分布
    print("\n⚠️  风险等级分布")
    print("-" * 40)
    all_risks = []
    for r in all_results:
        all_risks.extend(r['risk_levels'])
    risk_counter = Counter(all_risks)
    for risk, count in risk_counter.most_common():
        print(f"{risk}: {count} 个hunk")

    # 显示前20个文件的详细信息
    print("\n📋 前20个文件详细信息")
    print("=" * 80)
    for i, result in enumerate(all_results[:20]):
        print(f"\n{i+1}. {result['filename']}")
        print(f"   状态: {result['resolution_status']}")
        print(f"   Hunks: {result['resolved_hunks']}/{result['total_hunks']} ({result['success_rate']})")
        if result['risk_levels']:
            print(f"   风险等级: {', '.join(result['risk_levels'][:3])}")
        if result['conflict_types']:
            print(f"   主要冲突: {result['conflict_types'][0][:60]}...")

    # 保存完整结果到文件
    with open('analysis_summary.json', 'w', encoding='utf-8') as f:
        json.dump({
            "summary": {
                "total_files": len(all_results),
                "total_hunks": total_hunks,
                "resolved_hunks": resolved_hunks,
                "unresolved_hunks": unresolved_hunks,
                "overall_success_rate": f"{(resolved_hunks/total_hunks*100):.1f}%" if total_hunks > 0 else "N/A",
                "status_distribution": dict(status_counter),
                "risk_distribution": dict(risk_counter)
            },
            "detailed_results": all_results
        }, f, ensure_ascii=False, indent=2)

    print(f"\n✅ 分析完成！详细结果已保存到 analysis_summary.json")

if __name__ == "__main__":
    main()