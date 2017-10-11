utils = require 'mp.utils'
msg = require 'mp.msg'
lfs = require 'lfs'

local function get_fname(format, default_name)
    local cwd = mp.get_property("working-directory")
    if (cwd:find("://") ~= nil) then
        cwd = os.getenv('HOME')
    end
    local path = ""
    local screenshot_dir = mp.get_property('screenshot-directory', '')
    if not (screenshot_dir == "") then
        path = screenshot_dir
    else
        path = cwd
    end
    path = path:gsub('^(~)', os.getenv('HOME'))
    msg.debug('cwd: '..cwd..' path: '..path)
    lfs.mkdir(path)
    local basename = default_name
    local direntries = utils.readdir(path, "files")
    local ftable = {}
    for i = 1, #direntries do
        ftable[direntries[i]] = 1
    end
    local fname = ""
    for i=1, 9999 do
        local f = string.format('%s%04d.%s', basename, i, format)
        if ftable[f] == nil then
            msg.debug('fname: '..f)
            return f, path, cwd
        end
    end
    return nil
end

mp.register_script_message("screenshot_write_inaccurate", function (stype)
    local fname, path = get_fname("png", "mpv-shot")
    if not (fname == nil) then
        -- local pngfname = fname .. "_tempshot_.png"
		local fpath = utils.join_path(path, fname)
        -- local pngfpath = utils.join_path(path, pngfname)
        -- mp.commandv("screenshot-to-file", pngfpath, stype)
        mp.commandv("screenshot-to-file", fpath, stype)
        mp.commandv("show-text", string.format("Screenshot: '%s'", fname))
		mp.commandv("run", "/usr/bin/env", "denoise-squash-png", "--quiet", "--gpu=1", "--w2x=caffe", "--noise-level=2", "--type=anime", fpath);
    end
end)

mp.register_script_message("screenshot_write_accurate", function (stype)
    local fname, path = get_fname("png", "mpv-shot")
    if not (fname == nil) then
		local fpath = utils.join_path(path, fname)
        mp.commandv("screenshot-to-file", fpath, stype)
        mp.commandv("show-text", string.format("Screenshot: '%s'", fname))
		mp.commandv("run", "/usr/bin/env", "crush-png", fpath, "--quiet");
    end
end)

