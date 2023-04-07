import serial
import time
import termcolor

ser = serial.Serial("COM3", 9600)

while True:
    data = ser.read_all().decode()
    if data != "":
        if data == "9999,99,99,99":
            bb = 0
            sb = 0
            rb = 0
            is_bb = False
            is_sb = False
            is_rb = False
            is_rep = False
            rep = 0
            counter = 0
            _counter = 0
            credit = 0
            in_credit = 0
            rep_flg = False
            bonus_flg = False
            print(termcolor.colored("SB  " + str(sb), 'blue') +
                  "    " + termcolor.colored("GAME", 'yellow'))
            print(termcolor.colored("BB  " + str(bb), 'red') +
                  "    " + termcolor.colored(str(counter).rjust(4), 'yellow'))
            print(termcolor.colored("RB  " + str(rb), 'green'))
            print(termcolor.colored("REP " + str(rep), 'yellow'))
            print("差枚数" + termcolor.colored(str(credit).rjust(8),
                                            'blue' if credit >= 0 else 'red'))
            print("機械割" + (str(0.0) + "%").rjust(8))
            print("総回転数" + str(_counter).rjust(8))
            print("ボーナス確立" + (str(0.0) + "%").rjust(8))
            print("リプレイ確率" + (str(0.0) + "%").rjust(8))
            print(
                "|      |      |      |\n")
            print(
                "|      |      |      |\n")
            print(
                "|      |      |      |")
            print("\033[14A", end="")
        else:

            def nsum(base, offset):
                if base+offset == 21:
                    return 0
                elif base+offset == -1:
                    return 20
                else:
                    return base+offset

            Lreel = [0, 2, 6, 3, 2, 4, 2, 3, 1, 5, 2,
                     6, 3, 2, 4, 2, 3, 1, 2, 3, 5]
            Creel = [0, 3, 4, 2, 6, 3, 4, 2, 1, 3, 4,
                     2, 6, 3, 4, 2, 1, 3, 4, 2, 5]
            Rreel = [0, 1, 6, 2, 3, 5, 6, 2, 3, 5, 6,
                     2, 3, 5, 6, 2, 3, 5, 6, 2, 3]
            position = [[Lreel[nsum(int(data.split(",")[1]), -1)], Creel[nsum(int(data.split(",")[2]), -1)], Rreel[nsum(int(data.split(",")[3]), -1)]],
                        [Lreel[nsum(int(data.split(",")[1]), 0)], Creel[nsum(
                            int(data.split(",")[2]), 0)], Rreel[nsum(int(data.split(",")[3]), 0)]],
                        [Lreel[nsum(int(data.split(",")[1]), 1)], Creel[nsum(int(data.split(",")[2]), 1)], Rreel[nsum(int(data.split(",")[3]), 1)]]]

            def id2name(id):
                return ["7     ", "Bar   ", "Grape ", "REP   ", "Cherry", "Clown ", "Bell  "][int(id)]

            mode = int(data.split(",")[0][0])
            FreeSpin = int(data.split(",")[0][1])
            target = int(data.split(",")[0][2])
            payline = int(data.split(",")[0][3])
            if mode == 0:
                is_rep = False
                is_bb = False
                is_sb = False
                is_rb = False
                if bonus_flg:
                    bonus_flg = False
                else:
                    counter += 1
                    _counter += 1
                    if rep_flg:
                        rep_flg = False
                    else:
                        credit -= 3
                        in_credit += 3
            if mode == 1 and FreeSpin == 4:
                _counter += 1
                bonus_flg = True
                bb += 1
                is_bb = True
                counter = 0
                credit -= 3
                in_credit += 3
            elif mode == 2 and FreeSpin == 2:
                _counter += 1
                bonus_flg = True
                rb += 1
                is_rb = True
                counter = 0
                credit -= 3
                in_credit += 3
            elif mode == 3 and FreeSpin == 4:
                if is_rep:
                    sb += 1
                else:
                    is_rep = True
                    _counter += 1
                    bonus_flg = True
                    sb += 1
                    is_sb = True
                    counter = 0
                    credit -= 3
                    in_credit += 3
            if target == 1:
                credit += 6
            elif target == 3:
                rep += 1
                rep_flg = True
            if not mode == 0:
                print(termcolor.colored("SB  " + str(sb), 'blue', on_color="on_white") + "    " + termcolor.colored("LAST", 'red')
                      if is_sb else termcolor.colored("SB  " + str(sb), 'blue') + "    " + termcolor.colored("LAST", 'red'))

                print(termcolor.colored("BB  " + str(bb), 'red', on_color="on_white") + "    " + termcolor.colored(str(FreeSpin).rjust(4), 'red') if is_bb else termcolor.colored("BB  " + str(bb), 'red') + "    " + termcolor.colored(str(FreeSpin).rjust(4), 'red')
                      )
            else:
                print(termcolor.colored("SB  " + str(sb), 'blue', on_color="on_white") + "    " + termcolor.colored("GAME", 'yellow') if is_sb else termcolor.colored("SB  " + str(sb), 'blue') +
                      "    " + termcolor.colored("GAME", 'yellow'))
                print(termcolor.colored("BB  " + str(bb), 'red', on_color="on_white") + "    " + termcolor.colored(str(counter).rjust(4), 'yellow') if is_bb else termcolor.colored("BB  " + str(bb), 'red') +
                      "    " + termcolor.colored(str(counter).rjust(4), 'yellow'))
            print(termcolor.colored("RB  " + str(rb), 'green', on_color="on_white")
                  if is_rb else termcolor.colored("RB  " + str(rb), 'green'))
            print(termcolor.colored("REP " + str(rep), 'yellow'))
            print("差枚数" + termcolor.colored(str(credit).rjust(8),
                                            'blue' if credit >= 0 else 'red'))
            try:
                print(
                    "機械割" + (str(round(100*((in_credit + credit)/in_credit), 2)) + "%").rjust(8))
            except ZeroDivisionError:
                print("機械割" + (str(0.0) + "%").rjust(8))
            print("総回転数" + str(_counter).rjust(8))
            print(
                "ボーナス確立" + (str(round(100*((sb + bb + rb)/_counter), 2)) + "%").rjust(8))
            print("リプレイ確率" + (str(round(100*(rep/_counter), 2)) + "%").rjust(8))
            print("|" + termcolor.colored(id2name(position[0][0]), "white" if target == 0 else "red" if payline in (2, 4) else "white") + "|" + termcolor.colored(id2name(
                position[0][1]), "white" if target == 0 else "red" if payline == 2 else "white") + "|" + termcolor.colored(id2name(position[0][2]), "white" if target == 0 else "red" if payline in (2, 3) else "white") + "|" + "\n")
            print("|" + termcolor.colored(id2name(position[1][0]), "white" if target == 0 else "red" if payline == 1 else "white") + "|" + termcolor.colored(id2name(
                position[1][1]), "white" if target == 0 else "red" if payline in (1, 3, 4) else "white") + "|" + termcolor.colored(id2name(position[1][2]), "white" if target == 0 else "red" if payline == 1 else "white") + "|" + "\n")
            print("|" + termcolor.colored(id2name(position[2][0]), "white" if target == 0 else "red" if payline in (0, 3) else "white") + "|" + termcolor.colored(id2name(
                position[2][1]), "white" if target == 0 else "red" if payline == 0 else "white") + "|" + termcolor.colored(id2name(position[2][2]), "white" if target == 0 else "red" if payline in (0, 4) else "white") + "|")
            print("\033[14A", end="")
    else:
        time.sleep(0.0001)
    if data == b'\r':
        break
ser.close()
