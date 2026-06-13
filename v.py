import re

def fix_peashooter_perfect(input_file, output_file):
    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()

    tracks = re.findall(r'<track>.*?</track>', content, re.DOTALL)
    fixed_tracks = []

    # Danh sách các track KHÔNG ĐƯỢC ĐƯA VỀ FRAME 0 (Bọn này là blink/mouth, đưa về là lỗi đè hình)
    blacklist_tracks = [
        'anim_blink', 'idle_shoot_blink', 'idle_mouth', 
        'anim_full_idle', 'anim_shooting', 'anim_head_idle', 'anim_idle'
    ]

    for track in tracks:
        track_name = re.search(r'<name>(.*?)</name>', track).group(1)
        frames = re.findall(r'<t>.*?</t>', track, re.DOTALL)

        # 1. Xử lý các track bộ phận chính (Thân, Lá, Đầu)
        if track_name not in blacklist_tracks:
            # Tìm frame đầu tiên thực sự có tọa độ <x> và hình ảnh <i>
            valid_frame_data = None
            for frame in frames:
                if '<x>' in frame and '<i>' in frame:
                    valid_frame_data = frame
                    break
            
            # Đưa dữ liệu gốc về Frame 0 để không bị mất hình vài chục frame đầu
            if valid_frame_data and len(frames) > 0:
                clean_frame = re.sub(r'<f>-1</f>', '<f>0</f>', valid_frame_data)
                if '<f>' not in clean_frame:
                    clean_frame = clean_frame.replace('</t>', '<f>0</f></t>')
                frames[0] = clean_frame

            # Xóa các thẻ ẩn <f>-1</f> tai hại ở giữa animation (ví dụ các frame từ 29 đến 73)
            for i in range(1, len(frames)):
                if '<f>-1</f>' in frames[i]:
                    if frames[i] == '<t><f>-1</f></t>':
                        frames[i] = '<t></t>'
                    else:
                        frames[i] = frames[i].replace('<f>-1</f>', '')
        
        # 2. Xử lý riêng cho các track Nháy mắt & Mồm (Không cho hiện tràn lan)
        else:
            # Các track này ở Frame 0 BẮT BUỘC phải ẩn (-1) để khi nào cần mới hiện
            if len(frames) > 0 and track_name in ['anim_blink', 'idle_shoot_blink']:
                frames[0] = '<t><f>-1</f></t>'

        # Dựng lại cấu trúc track
        new_track = f"<track>\n<name>{track_name}</name>\n" + "\n".join(frames) + "\n</track>"
        fixed_tracks.append(new_track)

    # Đóng gói file hoàn chỉnh
    fps = re.search(r'<fps>\d+</fps>', content).group(0) if '<fps>' in content else '<fps>12</fps>'
    final_xml = fps + "\n" + "\n".join(fixed_tracks)
    
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(final_xml)
    print("Đã tách biệt bộ phận và blink! File chuẩn peashooter_perfect.txt đã sẵn sàng.")

# Chạy tool
fix_peashooter_perfect('peashooter.txt', 'peashooter_perfect.txt')