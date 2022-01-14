
local SEEN_TABLES = {}
local LAST_RELOAD_TIMES = {}
local IGNORE_MODULES = {}


local function table_keys(t)
	local res = {}
	for k in pairs(t) do
		res[#res + 1] = k
	end
	return res
end

local function table_find(t, value)
	for k, v in pairs(t) do
		if v == value then
			return k
		end
	end
	return nil
end

local function update_table(old, new)
	if table_find(SEEN_TABLES, old) ~= nil then
		return
	end
	SEEN_TABLES[#SEEN_TABLES + 1] = old

	local keys = table_keys(old)
	for i, key in ipairs(keys) do
		local old_obj = old[key]
		local new_obj = new[key]
		if new_obj ~= nil then
			local old_type = type(old_obj)
			local new_type = type(new_obj)
			if old_type == new_type then
				if old_type == 'function' then
					old[key] = new_obj
				elseif old_type == 'table' then
					update_table(old_obj, new_obj)
				end
			else
				old[key] = new_obj
			end
		end
	end
end


local function log(...)
	print('[retroreload]:', ...)
end

local function retroreload(module)
	if type(module) ~= 'table' then
		log('module is not table, ignored', module)
		return
	end

	local module_name = table_find(package.loaded, module)
	if module_name == nil then
		log('cannot find module, ignored', module)
		return
	end

	package.loaded[module_name] = nil
	local require_res, new_module = pcall(require, module_name)
	if not require_res then
		log('load failed', new_module)
		package.loaded[module_name] = module
		return
	end
	if type(new_module) ~= 'table' then
		log('new_module is not table, ignored', new_module)
		package.loaded[module_name] = module
		return
	end
	log('update module', module_name)
	SEEN_TABLES = {}
	update_table(module, new_module)
	SEEN_TABLES = {}
	package.loaded[module_name] = module
end

local function is_module_outdated(module, module_name)
	-- need os.path.getmtime in python
	return true
end

local function retroreload_module_name(module_name)
	local module = package.loaded[module_name]
	if module then
		if is_module_outdated(module, module_name) then
			retroreload(module)
			LAST_RELOAD_TIMES[module_name] = os.time()
		end
	end
end

local function retroreload_module_names(module_names)
	for i, module_name in ipairs(module_names) do
		retroreload_module_name(module_name)
	end
end

local function retroreload_script_folder(script_folder)
	local reload_module_names = {}
	for module_name, module in pairs(package.loaded) do
		-- need os.path.abspath and __file__ in python
	end
	retroreload_module_names(reload_module_names)
end

local function retroreload_script_folders(script_folders)
	for i, script_folder in ipairs(script_folders) do
		retroreload_script_folder(script_folder)
	end
end

local function should_reload_module(module_name)
	if module_name == 'retroreload' then
		return false
	end
	local builtin_modules = {
		'_G', 'package', 'coroutine', 'table', 'io', 'os', 'string',
		'math', 'utf8', 'debug'
	}
	if table_find(builtin_modules, module_name) then
		return false
	end
	if table_find(IGNORE_MODULES, module_name) then
		return false
	end
	for i, ignore_module in ipairs(IGNORE_MODULES) do
		local module_prefix = ignore_module .. '.'
		if module_name:sub(1, #module_prefix) == module_prefix then
			return false
		end
	end
	return true
end

local function retroreload_all()
	local reload_module_names = {}
	for module_name, module in pairs(package.loaded) do
		if should_reload_module(module_name) then
			reload_module_names[#reload_module_names + 1] = module_name
		end
	end
	retroreload_module_names(reload_module_names)
end

local function retroreload_add_ignores(module_names)
	for _, module_name in ipairs(module_names) do
		if not table_find(IGNORE_MODULES, module_name) then
			IGNORE_MODULES[#IGNORE_MODULES+1] = module_name
		end
	end
end

return {
	retroreload = retroreload,
	retroreload_module_name = retroreload_module_name,
	retroreload_module_names = retroreload_module_names,
	retroreload_script_folder = retroreload_script_folder,
	retroreload_script_folders = retroreload_script_folders,
	retroreload_all = retroreload_all,
	retroreload_add_ignores = retroreload_add_ignores,
}
