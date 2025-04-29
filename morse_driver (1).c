#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/slab.h>
#include "morse_driver.h"

static int morse_major;
static char *morse_input_buffer = NULL;
static DEFINE_MUTEX(morse_mutex);

const char *morse_code[] = {
    ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---",
    "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-",
    "..-", "...-", ".--", "-..-", "-.--", "--..",".....","-----"
};

static char decode_morse_char(const char *morse) {
    int i;
    for (i = 0; i < 28; i++) {
        if (strcmp(morse, morse_code[i]) == 0) {
            if (i < 26)
                return 'A' + i;
            else if (i == 26)
                return ' ';
            else if (i == 27)
                return '\n';
        }
    }
    return '?';
}

static void write_to_file(const char *decoded_chars) {
    struct file *f;
    loff_t pos = 0;
    ssize_t ret;

    f = filp_open("/home/prachi/Downloads/os_project_copy/morse_driver.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (IS_ERR(f)) {
        printk(KERN_ERR "Morse Driver: Failed to open file\n");
        return;
    }

    pos = f->f_pos;
    ret = kernel_write(f, decoded_chars, strlen(decoded_chars), &pos);
    if (ret < 0) {
        printk(KERN_ERR "Morse Driver: Failed to write to file\n");
    } else {
        f->f_pos = pos;
    }

    filp_close(f, NULL);
}

static void decode_full_morse(void) {
    char decoded[256] = {0};
    char morse_letter[16];
    int i = 0, j = 0, k = 0;

    if (!morse_input_buffer)
        return;

    while (morse_input_buffer[i] != '\0') {
        if (morse_input_buffer[i] != ' ') {
            if (j < sizeof(morse_letter) - 1)
                morse_letter[j++] = morse_input_buffer[i];
        } else {
            if (j > 0) {
                morse_letter[j] = '\0';
                if (strspn(morse_letter, ".-") == strlen(morse_letter)) {
                    decoded[k++] = decode_morse_char(morse_letter);
                }
                j = 0;
            }
        }
        i++;
    }

    if (j > 0) {
        morse_letter[j] = '\0';
        if (strspn(morse_letter, ".-") == strlen(morse_letter)) {
            decoded[k++] = decode_morse_char(morse_letter);
        }
    }

    if (k > 0) {
        decoded[k] = '\0';
        printk(KERN_INFO "Morse Driver: Decoded string: %s", decoded);
        write_to_file(decoded);
    } else {
        printk(KERN_INFO "Morse Driver: No valid Morse code to decode\n");
    }
}

static long morse_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    char user_buf[256];

    switch (cmd) {
        case MORSE_IOC_SEND:
            if (copy_from_user(user_buf, (char __user *)arg, sizeof(user_buf) - 1))
                return -EFAULT;
            user_buf[sizeof(user_buf) - 1] = '\0';

            printk(KERN_INFO "Morse Driver: Received Morse code: %s\n", user_buf);

            mutex_lock(&morse_mutex);
            kfree(morse_input_buffer);
            morse_input_buffer = kstrdup(user_buf, GFP_KERNEL);
            mutex_unlock(&morse_mutex);

            decode_full_morse();
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

static const struct file_operations morse_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = morse_ioctl,
};

static int __init morse_driver_init(void) {
    morse_major = register_chrdev(0, "morse_driver", &morse_fops);
    if (morse_major < 0) {
        printk(KERN_ALERT "Morse Driver: Failed to register device\n");
        return morse_major;
    }

    printk(KERN_INFO "Morse Driver: Registered with major number %d\n", morse_major);
    return 0;
}

static void __exit morse_driver_exit(void) {
    mutex_lock(&morse_mutex);
    kfree(morse_input_buffer);
    mutex_unlock(&morse_mutex);

    unregister_chrdev(morse_major, "morse_driver");
    printk(KERN_INFO "Morse Driver: Unregistered\n");
}

module_init(morse_driver_init);
module_exit(morse_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prachi Raj");
MODULE_DESCRIPTION("Buffered Morse Code Decoder Driver");
MODULE_VERSION("2.0");
